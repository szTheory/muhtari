#!/bin/sh
# Applies the GitHub repository settings this project relies on. Idempotent;
# run once after the repository exists and again at each milestone tune-up.
#
#   scripts/apply-repo-settings.sh [--check] [owner/repo]
#
# --check prints settings that differ from this file and exits 1 if any do.
# Needs: gh, authenticated with admin rights on the repository.
#
# Why squash title = PR title and squash body = PR body: GSD's ship step pushes
# a commit whose message carries a CI-skip token. If the squash message listed
# the branch's commits, that token would land on main and suppress the CI and
# release runs that main depends on.
set -eu

check=0
if [ "${1:-}" = --check ]; then check=1; shift; fi
repo=${1:-$(gh repo view --json nameWithOwner -q .nameWithOwner)}

# Merge settings: name=expected value, as the repository API reports them.
MERGE_SETTINGS='allow_squash_merge=true
allow_merge_commit=false
allow_rebase_merge=false
squash_merge_commit_title=PR_TITLE
squash_merge_commit_message=PR_BODY
allow_auto_merge=true
delete_branch_on_merge=true
allow_update_branch=true
has_wiki=false
has_projects=false'

LABELS='bug:d73a4a:Something is not working
compatibility:e99695:A game or homebrew title misbehaves
enhancement:a2eeef:New capability or improvement
docs:0075ca:Documentation
performance:fbca04:Speed, latency or memory
needs-info:d876e3:Waiting on the reporter
nightly:5319e7:Rolling nightly failure report
digest:c5def5:Weekly triage digest'

if [ "$check" = 1 ]; then
  drift=0
  current=$(gh api "repos/$repo")
  printf '%s\n' "$MERGE_SETTINGS" | while IFS='=' read -r key want; do
    have=$(printf '%s' "$current" | jq -r ".$key")
    [ "$have" = "$want" ] || { printf 'drift: %s is %s, want %s\n' "$key" "$have" "$want"; exit 1; }
  done || drift=1
  for path in secret_scanning secret_scanning_push_protection; do
    have=$(printf '%s' "$current" | jq -r ".security_and_analysis.$path.status")
    [ "$have" = enabled ] || { printf 'drift: %s is %s, want enabled\n' "$path" "$have"; drift=1; }
  done
  have=$(gh api "repos/$repo/actions/permissions/workflow" -q .default_workflow_permissions)
  [ "$have" = read ] || { printf 'drift: default_workflow_permissions is %s, want read\n' "$have"; drift=1; }
  # GitHub then refuses any action not pinned by full commit SHA.
  have=$(gh api "repos/$repo/actions/permissions" -q .sha_pinning_required)
  [ "$have" = true ] || { printf 'drift: sha_pinning_required is %s, want true\n' "$have"; drift=1; }
  [ "$drift" = 0 ] && echo "no drift on $repo"
  exit "$drift"
fi

set --
for setting in $MERGE_SETTINGS; do
  case $setting in
    *=true | *=false) set -- "$@" -F "$setting" ;;
    *) set -- "$@" -f "$setting" ;;
  esac
done
gh api -X PATCH "repos/$repo" --silent "$@"

# Secret scanning with push protection (free for public repositories).
gh api -X PATCH "repos/$repo" --silent --input - <<'JSON'
{"security_and_analysis": {"secret_scanning": {"status": "enabled"},
                           "secret_scanning_push_protection": {"status": "enabled"}}}
JSON

# Private vulnerability reporting, Dependabot alerts and security updates.
gh api -X PUT "repos/$repo/private-vulnerability-reporting" --silent
gh api -X PUT "repos/$repo/vulnerability-alerts" --silent
gh api -X PUT "repos/$repo/automated-security-fixes" --silent

# Actions: SHA-pinned actions only; read-only default token; Actions cannot approve PRs.
gh api -X PUT "repos/$repo/actions/permissions" --silent -F enabled=true -f allowed_actions=all -F sha_pinning_required=true
gh api -X PUT "repos/$repo/actions/permissions/workflow" --silent \
  -f default_workflow_permissions=read -F can_approve_pull_request_reviews=false

# Immutable releases: tags and assets are locked once a release is published.
gh api -X PUT "repos/$repo/immutable-releases" --silent

# Labels used by the issue forms, the nightly report and the weekly digest.
printf '%s\n' "$LABELS" | while IFS=: read -r name colour description; do
  gh label create "$name" --repo "$repo" --color "$colour" --description "$description" --force >/dev/null
done

# The branch ruleset (required checks, squash only, no force push) is a
# committed file that phase 1 adds together with the CI it requires, and is
# applied once a run has reported the required check.
ruleset=.github/rulesets/main.json
if [ -f "$ruleset" ]; then
  name=$(sed -n 's/^[[:space:]]*"name"[[:space:]]*:[[:space:]]*"\([^"]*\)".*/\1/p' "$ruleset" | head -n 1)
  id=$(gh api "repos/$repo/rulesets" -q ".[] | select(.name == \"$name\") | .id")
  if [ -n "$id" ]; then
    gh api -X PUT "repos/$repo/rulesets/$id" --silent --input "$ruleset"
  else
    gh api -X POST "repos/$repo/rulesets" --silent --input "$ruleset"
  fi
  echo "ruleset '$name' applied"
fi

echo "repository settings applied to $repo"
