#!/bin/sh
# GitHub REST run/attempt/job identities, UTC windows, and file-based report bodies.
# API contracts: docs.github.com/en/rest/actions/{workflow-runs,workflow-jobs}.
set -eu
export TZ=UTC
fail() { echo "digest: $*" >&2; exit 1; }
[ "$#" -ge 2 ] || fail 'usage: collect OUT START END | preview OUT [START END] | publish OUT'
operation=$1; out=$2; shift 2
case "$operation" in collect|preview|publish) ;; *) fail 'unknown operation' ;; esac
repo=${GITHUB_REPOSITORY:-$(gh repo view --json nameWithOwner -q .nameWithOwner)}
printf '%s\n' "$repo" | LC_ALL=C grep -Eq '^[A-Za-z0-9_.-]+/[A-Za-z0-9_.-]+$' || fail 'invalid repository'
mkdir -p "$out"
out=$(cd "$out" && pwd)
gh api "repos/$repo" > "$out/repository.json" || fail 'required repository API failed'
jq -e '.private == false and (.default_branch | type == "string")' "$out/repository.json" >/dev/null || fail 'public repository required'
now=$(date -u +%Y-%m-%dT%H:%M:%SZ)
# Each API command is checked before parsing; no pipeline hides an earlier failure.
pages() {
  gh api --paginate "$1" > "$2.raw" || fail "required API failed: $1"
  jq -s '.' "$2.raw" > "$2" || fail 'required API JSON malformed'
  rm "$2.raw"
}
if [ "$operation" = publish ]; then
  branch=$(jq -r .default_branch "$out/repository.json")
  [ "${GITHUB_ACTIONS:-}" = true ] && [ "${GITHUB_REF:-}" = "refs/heads/$branch" ] || fail 'forbidden publisher ref'
  case "${GITHUB_EVENT_NAME:-}" in
    schedule) ;;
    workflow_dispatch) [ "${DIGEST_PUBLISH:-false}" = true ] || fail 'forbidden publisher input' ;;
    *) fail 'forbidden publisher event' ;;
  esac
  [ -s "$out/report.md" ] && [ -s "$out/evidence.json" ] || fail 'required report missing'
  [ "$(jq -r .repo "$out/evidence.json")" = "$repo" ] || fail 'required report repository mismatch'
  gh issue list --repo "$repo" --label digest --state open --limit 1000 --json number,body > "$out/digests.json" || fail 'required digest list failed'
  count=$(jq 'length' "$out/digests.json")
  [ "$count" -le 1 ] || fail 'duplicate digest issues'
  cp "$out/monthly.md" "$out/publish-month.md"
  if [ "$count" = 1 ]; then
    jq -r '.[0].body' "$out/digests.json" > "$out/prior.md"
    awk '/^<!-- digest-month:start -->$/{keep=1} keep{print} /^<!-- digest-month:end -->$/{keep=0}' "$out/prior.md" > "$out/prior-month.md"
    if [ -s "$out/prior-month.md" ]; then
      [ "$(grep -c '^<!-- digest-month:' "$out/prior-month.md")" = 2 ] || fail 'required previous monthly delimiters malformed'
      # A partial month never erases the most recent complete month.
      complete=$(jq -r --arg now "$now" '.end <= $now and (.start|endswith("-01T00:00:00Z")) and (.end|endswith("-01T00:00:00Z")) and
        ((.end|fromdateiso8601)-(.start|fromdateiso8601) >= 2419200) and ((.end|fromdateiso8601)-(.start|fromdateiso8601) <= 2678400)' "$out/evidence.json")
      old_end=$(sed -n 's/^Window: .* to \([^ ]*\) UTC.*/\1/p' "$out/prior-month.md")
      new_end=$(jq -r .end "$out/evidence.json")
      if [ "$complete" != true ] || [ "$new_end" \< "$old_end" ]; then cp "$out/prior-month.md" "$out/publish-month.md"; fi
    fi
  fi
  { cat "$out/triage.md"; printf '\nLast successful report: %s\n\n' "$now"; cat "$out/publish-month.md"; } > "$out/publish.md"
  if [ "$count" = 0 ]; then
    gh issue create --repo "$repo" --label digest --title 'Weekly maintenance digest' --body-file "$out/publish.md" || fail 'required digest create failed'
  else
    number=$(jq -r '.[0].number' "$out/digests.json")
    gh issue edit "$number" --repo "$repo" --body-file "$out/publish.md" || fail 'required digest edit failed'
  fi
  printf '%s\n' "$now" > "$out/last-success.txt"
  exit 0
fi
if [ "$#" = 0 ] && [ "$operation" = preview ]; then
  set -- $(jq -nr --arg n "$now" '
    ($n|strptime("%Y-%m-%dT%H:%M:%SZ")) as $t |
    ($t|.[2]=1|.[3]=0|.[4]=0|.[5]=0|mktime) as $end |
    [(($end-1)|strftime("%Y-%m-01T00:00:00Z")), ($end|strftime("%Y-%m-%dT%H:%M:%SZ"))] | .[]')
fi
[ "$#" = 2 ] || fail 'START and END must both be UTC timestamps'
start=$1; end=$2
jq -en --arg a "$start" --arg b "$end" '
  def epoch: fromdateiso8601; ($a|epoch) < ($b|epoch)
  and ($a|epoch|strftime("%Y-%m-%dT%H:%M:%SZ"))==$a
  and ($b|epoch|strftime("%Y-%m-%dT%H:%M:%SZ"))==$b' >/dev/null || fail 'invalid UTC window'
# Listing every creation window also finds older runs rerun during the requested month.
# The REST listing cap is 1,000; split inclusive windows without losing boundary runs.
runs() (
  a=$1; b=$2; prefix=$3
  pages "repos/$repo/actions/runs?per_page=100&created=$a..$b" "$prefix.pages"
  jq -e 'all(.[]; (.total_count|type)=="number" and (.workflow_runs|type)=="array" and
    all(.workflow_runs[]; (.id|type)=="number" and .run_attempt>=1 and (.updated_at|type)=="string"))' "$prefix.pages" >/dev/null || fail 'required run schema'
  total=$(jq 'map(.total_count)|max // 0' "$prefix.pages")
  if [ "$total" -ge 1000 ]; then
    split=$(jq -nr --arg a "$a" --arg b "$b" '[$a,$b]|map(fromdateiso8601)|if .[1]-.[0]<2 then empty else ((add/2)|floor|todateiso8601) end')
    [ -n "$split" ] || fail 'truncated run listing at minimum window'
    runs "$a" "$split" "$prefix.a"
    runs "$split" "$b" "$prefix.b"
    jq -s 'add|unique_by(.id)' "$prefix.a.json" "$prefix.b.json" > "$prefix.json"
  else
    jq '[.[].workflow_runs[]]|unique_by(.id)' "$prefix.pages" > "$prefix.json"
    [ "$(jq length "$prefix.json")" = "$total" ] || fail 'truncated run pages'
  fi
)
birth=$(jq -r .created_at "$out/repository.json")
if [ "$birth" \< "$end" ]; then
  runs "$birth" "$end" "$out/runs"
else
  printf '[]\n' > "$out/runs.json"
fi
jq --arg a "$start" --arg b "$end" '[.[]|select(.updated_at >= $a and .created_at < $b)]|unique_by(.id)' "$out/runs.json" > "$out/window-runs.json"
: > "$out/jobs.ndjson"
jq -r '.[]|[.id,.run_attempt]|@tsv' "$out/window-runs.json" > "$out/attempts.tsv"
while read -r id attempts; do
  case "$id:$attempts" in *[!0-9:]*|:*) fail 'required run identity' ;; esac
  [ "$attempts" -ge 1 ] || fail 'required attempt count'
  attempt=1
  while [ "$attempt" -le "$attempts" ]; do
    pages "repos/$repo/actions/runs/$id/attempts/$attempt/jobs?per_page=100" "$out/jobs.pages"
    jq -e 'all(.[]; (.total_count|type)=="number" and (.jobs|type)=="array" and
      all(.jobs[]; (.id|type)=="number" and (.name|type)=="string" and (.labels|type)=="array"))' "$out/jobs.pages" >/dev/null || fail 'required jobs schema'
    jq '[.[].jobs[]]|unique_by(.id)' "$out/jobs.pages" > "$out/jobs.json"
    [ "$(jq length "$out/jobs.json")" = "$(jq 'map(.total_count)|max // 0' "$out/jobs.pages")" ] || fail 'truncated job pages'
    jq --argjson attempt "$attempt" --argjson run "$id" '.[]|. + {attempt:$attempt,run_id:$run}' "$out/jobs.json" >> "$out/jobs.ndjson"
    attempt=$((attempt + 1))
  done
done < "$out/attempts.tsv"
pages "repos/$repo/issues?state=open&per_page=100" "$out/issues.pages"
pages "repos/$repo/pulls?state=all&per_page=100" "$out/pulls.pages"
jq '[.[][]|select(has("pull_request")|not)]|unique_by(.number)' "$out/issues.pages" > "$out/issues.json"
jq '[.[][]]|unique_by(.number)' "$out/pulls.pages" > "$out/pulls.json"
: > "$out/release-checks.ndjson"
jq -r '.[]|select(.state=="open" and any(.labels[]; .name=="autorelease: pending"))|.head.sha' "$out/pulls.json" > "$out/release-shas"
while read -r sha; do
  pages "repos/$repo/commits/$sha/check-runs?per_page=100" "$out/checks.pages"
  jq --arg sha "$sha" '{sha:$sha,red:any(.[].check_runs[]; .conclusion=="failure" or .conclusion=="timed_out")}' "$out/checks.pages" >> "$out/release-checks.ndjson"
done < "$out/release-shas"
announcements=available
if gh api --paginate 'repos/actions/runner-images/issues?state=open&labels=Announcement&per_page=100' > "$out/announcements.raw"; then
  jq -s '[.[][]|select(.title|test("deprecat|brownout|remov";"i"))|{title,html_url,created_at}]' "$out/announcements.raw" > "$out/announcements.json" || announcements=unavailable
else
  announcements=unavailable
fi
if [ "$announcements" = unavailable ]; then printf '[]\n' > "$out/announcements.json"; fi
jq -n --arg start "$start" --arg end "$end" --arg generated "$now" --arg repo "$repo" --arg announcements "$announcements" \
  --slurpfile runs "$out/window-runs.json" --slurpfile jobs "$out/jobs.ndjson" --slurpfile issues "$out/issues.json" \
  --slurpfile prs "$out/pulls.json" --slurpfile checks "$out/release-checks.ndjson" --slurpfile news "$out/announcements.json" '
  {repo:$repo,start:$start,end:$end,generated:$generated,announcements:$announcements,
   runs:[$runs[0][]|{id,run_attempt,path,event,head_sha,html_url,created_at}],
   jobs:[$jobs[]|select(.started_at >= $start and .started_at < $end)|
     {id,run_id,attempt,name,conclusion,started_at,completed_at,html_url,labels,steps}]|unique_by([.run_id,.attempt,.id]),
   issues:[$issues[0][]|{number,title,labels,created_at,updated_at,html_url}],
   prs:[$prs[0][]|{number,title,labels,created_at,updated_at,merged_at,state,html_url,user:{login:.user.login},head:{sha:.head.sha}}],
   checks:$checks,news:$news[0]}' > "$out/evidence.json" || fail 'required evidence aggregation'
# Markdown escapes untrusted text; titles and branches never become shell source.
jq -r '
  def safe: tostring|gsub("&";"&amp;")|gsub("<";"&lt;")|gsub(">";"&gt;")|gsub("[\\r\\n|`]";" ");
  def age($date): ((.generated|fromdateiso8601)-($date|fromdateiso8601))/86400|floor;
  def major: try (.title|capture(" from v?(?<old>[0-9]+)[^ ]* to v?(?<new>[0-9]+)")|(.new|tonumber)>(.old|tonumber)) catch false;
  . as $e | "# Weekly maintenance digest\n\nGenerated: \(.generated)\nEvidence: \(.start) to \(.end) (UTC, end exclusive)\n",
  "## Triage\n\nReason | Age (days) | Evidence | Next action\n--- | ---: | --- | ---",
  (.issues[]|. as $i|select((.labels|length)==0 or any(.labels[];.name=="nightly"))|
    "\(if any(.labels[];.name=="nightly") then "Open nightly failure" else "Unlabelled issue" end) | \($e|age($i.created_at)) | [\(.title|safe)](\(.html_url)) | Inspect evidence and label or reproduce"),
  (.prs[]|select(.state=="open")|. as $p|($e|age($p.updated_at)) as $idle|
    (any(.labels[];.name=="autorelease: pending")) as $release|
    (any($e.checks[];.sha==$p.head.sha and .red)) as $red|
    select($idle>7 or ($release and (($e|age($p.created_at))>=1 or $red)) or
      (.user.login=="dependabot[bot]" and major))|
    "\(if $release then "Stale/red release PR" elif .user.login=="dependabot[bot]" then "Dependabot age/major (inspect release notes)" else "Idle PR" end) | \($idle) idle | [\(.title|safe)](\(.html_url)) | Review checks, release notes and next change"),
  (.news[]|. as $n|"Runner-image deprecation | \($e|age($n.created_at)) | [\(.title|safe)](\(.html_url)) | Compare announcement with workflow runner labels"),
  "\nRunner-image announcements: \(.announcements). Sources are restricted to this public repository and actions/runner-images.\n"' "$out/evidence.json" > "$out/triage.md"
jq -r '
  def seconds: if .started_at and .completed_at then ((.completed_at|fromdateiso8601)-(.started_at|fromdateiso8601)) else null end;
  def percentile($p): sort|.[((length*$p|ceil)-1)];
  . as $e | [.jobs[]|select(any(.steps[]?;.name=="Forced failure" and .conclusion=="failure"))|.run_id] as $controls |
  [.jobs[]|. as $j|($e.runs[]|select(.id==$j.run_id)) as $r|
    . + {seconds:seconds,category:(if (.name|test("(^| / )(changes|title|CI required)$")) then "planning/light"
      elif ($r.path|endswith("/release.yml")) and (.name|startswith("ci / ")) then "release-called"
      elif $r.path|endswith("/ci.yml") then "direct CI" else "other" end),control:($controls|index($j.run_id)!=null)}] as $jobs |
  [.prs[]|select(.merged_at >= $e.start and .merged_at < $e.end)]|length as $merged |
  "<!-- digest-month:start -->\n## Monthly CI efficiency\n\nWindow: \($e.start) to \($e.end) UTC (end exclusive). Generated: \($e.generated).",
  "\nRuns: \($e.runs|length); attempts with in-window jobs: \($jobs|unique_by([.run_id,.attempt])|length); jobs: \($jobs|length).",
  "Runner minutes: \(if ($jobs|length)==0 or any($jobs[];.seconds==null and .conclusion!="skipped") then "unavailable (no completed timestamp sample)" else ([$jobs[].seconds//0]|add//0)/60 end) (cancelled and intentional controls retained). Merged PRs: \($merged).",
  "Minutes per merged PR: \(if $merged==0 or any($jobs[];.seconds==null and .conclusion!="skipped") then "unavailable" else ([$jobs[].seconds//0]|add//0)/60/$merged end).",
  "Rerun-pass candidates: \([$jobs|group_by([.run_id,.name,.labels])[]|select(.[0].control|not)|select(any(.[];.conclusion=="failure"))|select(any(.[];.conclusion=="success" and .attempt>1))]|length) (candidate only; not proven flakes).",
  "\nCategory | Jobs | Cancelled | Failures | Intentional control jobs\n--- | ---: | ---: | ---: | ---:",
  ($jobs|group_by(.category)[]|"\(.[0].category) | \(length) | \(map(select(.conclusion=="cancelled"))|length) | \(map(select(.conclusion=="failure"))|length) | \(map(select(.control))|length)"),
  "\nJob / runner labels | n | p50 seconds | p95 seconds\n--- | ---: | ---: | ---:",
  ($jobs|map(select(.seconds!=null))|group_by([.name,.labels])[]|
    "\(.[0].name|gsub("[|\\r\\n<>`]";" ")) / \(.[0].labels|join(", ")) | \(length) | \(map(.seconds)|percentile(0.5)) | \(map(.seconds)|percentile(0.95))"),
  "\nObserved longest job: \(if ($jobs|length)==0 then "unavailable" else ($jobs|map(.seconds)|max) end) seconds; workflow DAG critical path unavailable (API does not expose needs edges).",
  "Runner queue and dependency wait unavailable separately; run creation is never treated as dependent-job queue time.",
  "Compile share unavailable: step names do not prove compiler-only time. CTest utilization unavailable: JUnit duration and matching CTest wall-time artifacts not collected.",
  "Cohorts above use job name and runner labels; image/tool/workload changes are not recorded by this API. Compare immutable run links and workflow commits before interpreting trends; sub-ten samples are provisional.",
  "Repository history is young; no 90-day redundancy conclusion. Missing retained logs/artifacts are unavailable, never zero. Intentional-control detection uses an observed failed Forced failure step; other controls remain unclassified.",
  "\nEvidence runs:", ($e.runs[]|"- [\(.id)](\(.html_url)) at \(.head_sha)"), "<!-- digest-month:end -->"' "$out/evidence.json" > "$out/monthly.md" || fail 'required monthly rendering'
cat "$out/triage.md" "$out/monthly.md" > "$out/report.md"
echo "digest: preview report generated"
