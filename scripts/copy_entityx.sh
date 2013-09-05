print "Will copy entityx files"
rsync -arvh --progress --exclude "*_test.cc" --exclude "python" \
--exclude "Manager.*" \
"/Users/davidwicks/Code/entityx/entityx/" "../src/entityx"
