#!/bin/bash

msg1="http://www.opennet.ru/docs/RUS/zlp/index.html
у нас изначально [hello.c info.sh main.c Makefile]"

msg2="мы описали Makefile и теперь при вызове утилиты 
[make] в этой дериктории выполнится, то что в этом 
файле написанно. Вызвав [make] без параметров будет 
решаться первая цель по тексту файла [make]

Мы можем вызвать [make] с аргументом [djoClean]
и будет решаться чепь для достижения этого результата

изменения в одном файле, если они не затрагивают другой
значит файл можно не перекомпилировать!"

echo "$msg1"
echo ""
echo "$msg2"
