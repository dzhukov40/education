#!/usr/bin/env bash

BASIC_AUTH_HEADER=${KIBANA_BASIC_AUTH_HEADER}
KIBANA_URL=${KIBANA_URL}

# args:
#  arg1: index pattern template name
# return: function use "echo" for return result
function createIndexPatternTemplate {
  FIELDS_PLACEHOLDER="TSLG_INDEX_PATTERN_FIELDS"
  CURRENT_DATE_PLACEHOLDER="TSLG_INDEX_PATTERN_CURRENT_DATE"

  INDEX_PATTERN_FIELDS_FILE="./${1}-fields.json"
  INDEX_PATTERN_TEMPLATE_FILE="./tslg-index-pattern-template.json"

  CURRENT_DATE=$(date '+%Y-%m-%dT%H:%M:%S.000Z')

  INDEX_PATTERN_FIELDS=$(cat $INDEX_PATTERN_FIELDS_FILE | tr -d '\r\n' | tr -d ' ' | sed -e 's/"/\\"/g')
  INDEX_PATTERN_TEMPLATE=$(cat $INDEX_PATTERN_TEMPLATE_FILE | tr -d '\r\n' | tr -d ' ')

  PATTERN="${INDEX_PATTERN_TEMPLATE/${FIELDS_PLACEHOLDER}/${INDEX_PATTERN_FIELDS}}"
  RESULT=$(echo $PATTERN | sed -e "s/$CURRENT_DATE_PLACEHOLDER/$CURRENT_DATE/g")

  echo $RESULT
}


# args:
#  arg1: index pattern template name
# return: function put result in "RESULT_OF_GET_INDEX_PATTERN_TEMPLATE" global variable
INDEX_PATTERN_TEMPLATES=() # global array for cache index template patterns
RESULT_OF_GET_INDEX_PATTERN_TEMPLATE=""
function getIndexPatternTemplate {
  # return index pattern template from cache is exist
  for index_pattern_template in "${INDEX_PATTERN_TEMPLATES[@]}"
  do
    NAME=${index_pattern_template%%|*}
    #echo "$NAME =? $1 \n"

    if [ "$NAME" = "$1" ]; then
      RESULT_OF_GET_INDEX_PATTERN_TEMPLATE="${index_pattern_template#*|}"
      return
    fi
  done

  # create index pattern template, put in cache and return as result value
  INDEX_PATTERN_TEMPLATE="$(createIndexPatternTemplate "$1")"
  INDEX_PATTERN_TEMPLATES+=("${1}|${INDEX_PATTERN_TEMPLATE}")

  RESULT_OF_GET_INDEX_PATTERN_TEMPLATE="$INDEX_PATTERN_TEMPLATE"
}


#---------------------------------------------------------------------------
# START SCRIPT
#---------------------------------------------------------------------------
INDEX_PATTERN_COUNT=0
FILE_PATH="index-patterns.txt"
REQUEST_FILE_NAME="index-patterns.ndjson"
rm -fr ${REQUEST_FILE_NAME}

TITLE_PLACEHOLDER="TSLG_INDEX_PATTERN_TITLE"
ID_PLACEHOLDER="TSLG_INDEX_PATTERN_ID"


while IFS=";" read -r TEMPLATE_NAME UUID INDEX_PATTERN
do
  if [ -z "$TEMPLATE_NAME" ]; then
    continue
  fi

  # trim
  TEMPLATE_NAME=${TEMPLATE_NAME//[[:blank:]]/} ; UUID=${UUID//[[:blank:]]/} ; INDEX_PATTERN=${INDEX_PATTERN//[[:blank:]]/}

  let "INDEX_PATTERN_COUNT += 1"
  getIndexPatternTemplate "$TEMPLATE_NAME"

  PATTERN_WITH_TITLE=$(echo $RESULT_OF_GET_INDEX_PATTERN_TEMPLATE | sed -e "s/$TITLE_PLACEHOLDER/$INDEX_PATTERN/g")
  PATTERN_WITH_TITLE_AND_ID=$(echo $PATTERN_WITH_TITLE | sed -e "s/$ID_PLACEHOLDER/$UUID/g"  )

  echo "$PATTERN_WITH_TITLE_AND_ID" >> $REQUEST_FILE_NAME

  echo "template name: $TEMPLATE_NAME , uuid: $UUID , index pattern: $INDEX_PATTERN"

done < "$FILE_PATH"


EXPORTED_COUNT_PLACEHOLDER="EXPORTED_COUNT"
END_OF_FILE_PATTERN='{"exportedCount":EXPORTED_COUNT,"missingRefCount":0,"missingReferences":[]}'

END_OF_FILE_WITH_EXPORTED_COUNT="${END_OF_FILE_PATTERN/${EXPORTED_COUNT_PLACEHOLDER}/${INDEX_PATTERN_COUNT}}"
echo "$END_OF_FILE_WITH_EXPORTED_COUNT" >> $REQUEST_FILE_NAME

STATUS_CODE=$(curl \
  -s \
  -o /dev/null \
  -w "%{http_code}" \
  -X POST $KIBANA_URL"/api/saved_objects/_import?overwrite=true" \
  -H "kbn-xsrf: true" \
  -H "securitytenant: global" \
  -H "Authorization: Basic "$BASIC_AUTH_HEADER \
  --form file=@index-patterns.ndjson \
  --compressed \
  $@)

printf $STATUS_CODE >result-saved-objects-import.txt
