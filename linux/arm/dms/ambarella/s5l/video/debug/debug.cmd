cat /proc/interrupts | grep vin && sleep && cat /proc/interrupts | grep vin
fps 30

grep 同时满足多个关键字和满足任意关键字
cat /proc/interrupts | grep -E "vin0|vout"&& echo "1s elapsed:" && sleep 1 && cat /proc/interrupts | grep -E "vin0|vout"

grep 同时满足多个关键字和满足任意关键字
① grep -E "word1|word2|word3"   file.txt
   满足任意条件（word1、word2和word3之一）将匹配。
② grep word1 file.txt | grep word2 |grep word3
   必须同时满足三个条件（word1、word2和word3）才匹配。
