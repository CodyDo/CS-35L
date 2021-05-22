#!/usr/bin/bash

# Allows bash to see hidden files (those that start with .)
shopt -s dotglob

###############################
### INITIAL ARGUMENT CHECKS ###
###############################                                             

# Checks if the recursive flag -r was used as the first argument. If
# it is, set up a variable flag for later use and shift
if [[ $1 =~ ^\-r$ ]]; then
    doRecursion=0
    shift
else
    doRecursion=1
fi

# Checks if # of arguments is greater than 1                                   
if [[ $# -gt 1 ]]; then
    echo "Error: Too many arguments. Please provide only one." 1>&2
    exit 1
fi

# If no arguments or if argument is '.' then set to current directory
# If not, then set it equal to the first argument
if [[ $# -eq 0 ]] || [[ $1 == '.' ]]; then
    dir=$(pwd)                
else
    dir=$1                    
fi

# If it ends in one or more '/'s, then remove it (needed to allow) helper
# function to work later. The double [] is needed because =~ is used
# (which signifies a regular expression)
if [[ "$dir" =~ /$ ]]; then
    dir=$(echo $dir | sed s/[/]*$//)
fi

# Checks if argument starts with a '-'
if [[ ${dir:0:1} == '-' ]]; then
    echo "Error: Argument cannot start with '-'." 1>&2
    exit 1
fi

# Checks if argument is a directory
if [[ ! -d "$dir" ]]; then
    echo "Error: $dir is not a valid directory." 1>&2
    exit 1
fi


###############################
###        FUNCTIONS        ###
###############################

# Checks to make sure the naming convention is appropriate
# Errors = True, No Errors = False
# $1 is the root folder and $2 is what is being checked
checkName() {
    # Composed of the 26 letters of alphabet, '.', '-', or '_'. Max of 14
    # The $1 in front is needed to ensure that path D is placed prior to
    # any new names
    if [[ ! $2 =~ $1/[A-Za-z_\.\-]{1,14}$ ]]; then
        return 0

    # Filename cannot start with -, if it does return false
    elif [[ $2 =~ $1/\- ]]; then
        return 0

    # Cannot start with '.' unless the name is solely '.' or '..' 
    elif [[ $2 =~ $1/\.  ]]; then
        if [[ ! $2 == "$1/." ]] && [ ! $2 == "$1/.." ]; then
            return 0
        
        else 
            return 1
        fi

    else
        return 1
    fi
}

# Checks if an item has repeated
checkRepeat() {
    # Store the first input before shifting to remove the first input
    # from the other array inputs
    # Repeat = True, No Repeat = False
    checkMe=($(echo $1 | tr '[:upper:]' '[:lower:]'))
    shift
    for i in $@; do
        if [[ "$checkMe" == "$i" ]]; then
            return 0
        fi
    done
    return 1
}


checkValid() {
    # Array of valid names. This array is unique to each "level" of files.
    # This ensures that only names within the same directory are compared
    # per recursive call.
    declare -a checkedNames=()

    for i in $1/*; do
        # Check if name is valid.
        if checkName $1 $i; then
            if [ -d $i ]; then
                echo "$i/" 1>&2
            else
                echo "$i" 1>&2
            fi
            
        # If name is valid then check if name already exists.
        # This should only run if name is valid and will only output
        # the repeated name once
        elif checkRepeat $i ${checkedNames[@]}; then
            if [ -d $i ]; then
                echo "$i/" 1>&2
            else
                echo "$i" 1>&2
            fi

        else
            # Add name to checkedNames if it's valid and unique
            checkedNames+=($(echo $i | tr '[:upper:]' '[:lower:]'))

            # RECURSIVE CODE. Checks if doRecursion was set. If yes,
            # recursively go through directories. If no, ignore
            if [ $doRecursion -eq 0 ]; then
                # Checks if i is a directory
                if [ -d $i]; then
                    # Checks if i can be read/accessed
                    if [ -r $i]; then
                        checkValid $i
                    else
                        echo "Error: Can't read $i. Read Permissions required." 1>&2
                    fi
                fi
            fi
        fi
    done
}


###############################
###       RUNNING THE       ###
###         PROGRAM         ###
###############################

# Change IFS so that path names with a space won't have issues
# Used a \n as it's likely to not show up
IFS='\n' 

checkValid $dir

# Resets the IFS to whitespace
unset IFS