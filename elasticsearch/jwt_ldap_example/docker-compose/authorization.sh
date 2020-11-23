#!/usr/bin/env bash

# login in kibana as "kibanaAdmin"
# http://127.0.0.1:5601?jwtToken=eyJhbGciOiJIUzI1NiJ9.eyJzdWIiOiJraWJhbmFBZG1pbiIsImlzcyI6Imh0dHBzOi8vbG9jYWxob3N0IiwiZXhwIjoxNjkwMDM4MjQ0fQ.pKOBygKY7IRIabhqBY6WCUzVul4TDSfSOVGMB_6b_x0

TOKENS=( "kibanaAdmin:eyJhbGciOiJIUzI1NiJ9.eyJzdWIiOiJraWJhbmFBZG1pbiIsImlzcyI6Imh0dHBzOi8vbG9jYWxob3N0IiwiZXhwIjoxNjkwMDM4MjQ0fQ.pKOBygKY7IRIabhqBY6WCUzVul4TDSfSOVGMB_6b_x0"
         "psantos:eyJhbGciOiJIUzI1NiJ9.eyJzdWIiOiJwc2FudG9zIiwiaXNzIjoiaHR0cHM6Ly9sb2NhbGhvc3QiLCJleHAiOjE2OTAwMjY1MDh9.jrKLhhDW5sANTelnTdRAYH2KAdLpfkAxxKbSINCakAs"
         "jroe:eyJhbGciOiJIUzI1NiJ9.eyJzdWIiOiJqcm9lIiwiaXNzIjoiaHR0cHM6Ly9sb2NhbGhvc3QiLCJleHAiOjE2OTAwMjY1MDh9.ibZkbbSSsywBSoj3O1e6rjPPI_ew6TpSAkcspZn9xFI" )


for token in "${TOKENS[@]}" ; do
    tokenName="${token%%:*}"
    tokenValue="${token##*:}"
    printf "==> %s: %s\n" "$tokenName" "$tokenValue"
    curl -XGET https://localhost:9200/_opendistro/_security/authinfo\?pretty -H "Authorization: Bearer $tokenValue"  --insecure
done


kibanaAdminToken=${${TOKENS[1]}##*:}
curl -XGET https://localhost:9200/_cat/nodes -H "Authorization: Bearer ${kibanaAdminToken}" --insecure

curl -XPUT "https://localhost:9200/new-index/_doc/1" -H 'Content-Type: application/json' -H "Authorization: Bearer ${kibanaAdminToken}" -d '{"title": "test_data"}' -k
curl -XPUT "https://localhost:9200/new-index2/_doc/1" -H 'Content-Type: application/json' -H "Authorization: Bearer ${kibanaAdminToken}" -d '{"title": "test_data2"}' -k
printf "\n---\n"
curl -XGET https://localhost:9200/_cat/indices -H "Authorization: Bearer ${kibanaAdminToken}" --insecure



