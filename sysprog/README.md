# How to test code
## Step 1
First of all you need to run run.sh and pass arg for fanotify
### Annotation
./run.sh _{some-path}_

## Step 2
Then run run_encoder.sh and pass full path to directory for encoder
## Caution
**Don't use necessary for you directories**
### Annotation
./run_encoder.sh _path-to-some-unnecessary-directory_

## Step 3
In _logs_ file you can find info about killed process
