if any(.[]; ((.filename // "") | contains("\n")) or ((.previous_filename // "") | contains("\n"))) then
  "(listing failed)"
else
  .[] | .filename, (.previous_filename // empty)
end
