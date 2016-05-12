#!/bin/bash

echo "CSC 252 Project 4 Submission Script"
echo "usage: bash $0 [extra credit trace file]"
echo ""

# Sanity Check
if [[ ! -d "src" || ! -e "Makefile" ]]; then
    echo "Error: Cannot locate project files"
    echo "Please copy script to your project directory and run like:"
    echo ""
    echo "    bash createSubmission.sh"
    echo ""
    exit 1
fi

echo "Enter your Net ID (e.g. cconkli4):"
read netid1

if [[ "$netid1" == "" ]]; then
    echo "Error: Blank Net ID"
    exit 1
fi

echo "Enter the Net ID of your partner (blank line for no partner):"
read netid2

if [[ "$netid2" == "" ]]; then
    outputname=$netid1
else
    outputname="$netid1-$netid2"
fi

# Create the submission directory
mkdir "submission_${outputname}"

# Move relevant files into submission directory
cp -rf src Makefile "submission_${outputname}/"
if [[ -e $1 ]]; then
    cp -rf $1 "submission_${outputname}/extracredit.trace"
fi


# Create the submission tarball
tar czf ${outputname}.tgz "submission_${outputname}/"

# Destroy the submission directory
rm -rf "submission_${outputname}/"

# Self-test
tar xzf ${outputname}.tgz
pushd "submission_${outputname}/" >& /dev/null
make cacheSim > /dev/null

if [ $? -ne 0 ]; then
  echo "Error in submission: make failed."
  exit 1
fi

popd >& /dev/null

# Cleanup
rm -rf "submission_${outputname}"

echo "Upload ${outputname}.tgz to Blackboard"
