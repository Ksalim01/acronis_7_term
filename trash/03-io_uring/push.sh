#!/bin/bash

git add solution.c
git commit -m "bugFix"
git push origin solutions

curl -s -X POST http://185.106.102.104:9091/submit/Ksalim01/03-io_uring
curl -s -X GET http://185.106.102.104:9091/results/Ksalim01/03-io_uring/last | jq
