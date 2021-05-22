#!/usr/bin/env bash

# PASSED: 28

# Ensures that spaces won't disrupt paths with spaces
# We are told "\n" won't be a character in filenames so it is used
IFS=$'\n'

# Recusion is set to false unless detected and changed later
doRecursion=1

###############################
###        FUNCTIONS        ###
###############################

# Get all files in the immediate directory and then call doEverything
function basicPoornames() {
    # Include IFS statement here again so that recursive calls of 
    # basicPoornames won't break with a space
    IFS=$'\n'
    # If # of args is 2, then recursive flag was input
    if [[ $# -eq 2 ]]; then
        directFiles=$(find "$1" -maxdepth 1 ! -samefile "$1")
        doEverything "${directFiles[@]}"
        find "$1" -type d ! -samefile "$1" -exec bash -c 'basicPoornames "{}" 0 ' \;
        # find "$1" -type d -maxdepth 1 ! -samefile "$1"| xargs basicPoornames
        # allFiles=$(find "$1" -type d  -maxdepth 999 ! -samefile "$1")
        # doEverything "${allFiles[@]}"
    else
        directFiles=$(find "$1" -maxdepth 1 ! -samefile "$1")
        doEverything "${directFiles[@]}"
    fi
}

# Checks $1 to see if it's alright. Returns 0 (true) if no.
function checkName() {
    # If there are spaces, replace them with *. These will get caught later
    # This is a roundabout way of doing this because when I implemented a
    # check for spaces/whitespace, it would incorrectly mark file names as
    # "bad"
    newCheck=$(echo "$1" | sed 's/ /\*/g')

    # If string is empty, that's fine return 1
    if [[ -z $1 ]]; then
        return 1

    # Composed of the 26 letters of alphabet, '.', '-', or '_'.
    elif [[ "$newCheck" =~ [^A-Za-z_\.\-] ]]; then
        return 0

    # Makes sure there is a max of 14 characters
    elif [[ "$newCheck" =~ [A-Za-z_\.\-]{15,} ]]; then
        return 0

    # Filename cannot start with -, if it does return false
    elif [[ "$newCheck" =~ ^\- ]]; then
        return 0

    # Cannot start with '.' unless the name is solely '.' or '..' 
    elif [[ "$newCheck" =~ ^\.  ]]; then
        if [[ ! "$newCheck" == "." ]] && [ ! "$newCheck" == ".." ]; then
            return 0
        
        fi
    fi

    return 1
}

function doEverything() {
    # allNames is an array of the names given to doEverything
    # duplicateNames stores any repeated names ignoring case
    # poorNames contains all the poor names
    declare -a allNames=()
    declare -a duplicateNames=()
    declare -a badNames=()
    declare -a uniqBadNames=()

    # Disable the wildcard * when adding cycling through all the
    # file names. This is prevent the function recalling everything
    # in the directory in the event a file is named '*' literally
    set -o noglob

    for i in $@; do
        # Add every argument into an array that's used to check for
        # unique names later
        allNames+=("$i")

        # Removes everything except the ending (aka the name to check)
        # Folder1/Folder2/checkMe << truncates all but checkMe
        checkMe=$(echo "$i" | sed -E "s/.*\/(.*)$/\1/ ")
        
        # If name doesn't fit proper conventions, add to badNames
        if checkName "$checkMe"; then
            badNames+=("$i")
        fi

    done

    # Reenables * for later use
    set +o noglob

    # Inputs all the names in the allNames array into uniq. -Di means
    # uniq will output any repeating lines, ignoring case
    duplicateNames=($(echo "${allNames[*]}" | tr ' ' '\n' | sort | uniq -Di))
    for i in "${duplicateNames[@]}"; do
        #If badNames already has it, don't add it
        if [[ ! " ${badNames[@]} " =~ " ${i} " ]]; then
            badNames+=("$i")
        fi
    done

    for i in "${badNames[@]}"; do
        # If it's a directory add a '/' at the end
        if [ -d "$i" ]; then
			echo "$i/"
		else
			echo "$i"
		fi
    done

}

###############################
###       RUNNING THE       ###
###         PROGRAM         ###
###############################

# Export commands for recursion
export -f basicPoornames
export -f doEverything
export -f checkName

# Chooses appropriate command based off the number of arguments
if [ $# -gt 0 ]; then
    # Checks if first command is recursive flag.
    if [[ $1 =~ ^\-r$ ]]; then
        # Yes recursion so if > 2 arguements, exit and output message
        if [ $# -le 2 ]; then
            # If -r is the only argument or $2 = "." run on pwd
            if [ $# -eq 1 ] || [[ $2 == "." ]]; then
                doRecursion=0
                basicPoornames $(pwd) $doRecursion
            else
                # If $2 is not a directory, exit and output message
                if [ -d $2 ]; then
                    # If $2 can't be read/accessed, output message (no exit)
                    if [ -r $2 ]; then
                        doRecursion=0
                        basicPoornames "$2" $doRecursion
                    else
                        echo "Error: Can't read/access directory" 1>&2
                    fi
                else
                    echo "Error: Input is not a valid directory" 1>&2
                    exit 1
                fi
            fi
        else
            echo "Error: Too many arguments." 1>&2
            exit 1
        fi
    
    else
        # No recursion so if > 1 argument, then exit and output message.
        if [ $# -eq 1 ]; then
            # If #1 is '.' then run on pwd
            if [[ $1 == "." ]]; then 
                basicPoornames $(pwd)
            else
                # If $1 is not a directory, exit and output message
                if [ -d $1 ]; then
                    # If $1 can't be read/accessed, output message (no exit)
                    if [ -r $1 ]; then
                        basicPoornames "$1"
                    else
                        echo "Error: Can't read/access directory" 1>&2
                    fi
                else
                    echo "Error: Input is not a valid directory" 1>&2
                    exit 1
                fi
            fi

        else
            echo "Error: Too many arguments." 1>&2
            exit 1
        fi
    fi

# If no arguments then use basicPoornames with pwd
else
    basicPoornames $(pwd)
fi