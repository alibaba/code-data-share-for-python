python -c 'import mod1'

rm -rf test.lst test.img
echo 'mod1' > test.lst
python -c 'import cds.dump; cds.dump.run_dump("test.lst", "test.img")'
PYCDSMODE=SHARE PYCDSARCHIVE=test.img python -c 'import mod1'

RESULT=$?
if [ $RESULT -eq 0 ]; then
  exit 1
fi
