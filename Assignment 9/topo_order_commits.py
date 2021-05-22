import os
import sys
import zlib


def topo_order_commits():
    branches_dict = dict()  # Dictionary {Hash: {List of Branch Names}}
    local_branches_list = list()  # List of all branch hashes
    commit_dict = dict()   # Dictionary {Hash: CommitNode of Parents/Children}
    root_hashes = list()  # List of root nodes
    order = list()  # List of topologically ordered commits

    get_branch_names(branches_dict, local_branches_list)
    commit_dict = build_commit_graph(root_hashes, local_branches_list)
    order = get_topo_ordered_commits(commit_dict, root_hashes)
    print_topo_ordered_commits_with_branch_names(commit_dict, order, branches_dict)


class CommitNode:
    def __init__(self, commit_hash):
        """
        :type commit_hash: str
        """
        self.commit_hash = commit_hash
        self.parents = set()
        self.children = set()


# PART 1:
# This function will find the .git directory by continuously searching
# directories of higher levels. If no .git is found or if it cannot be accessed
# then exit appropriately.
def find_git_repo():
    # os.getcwd() will get the directory that this python script is CALLED from
    # not where it's located (which is what we want.)
    path_name = os.getcwd()

    while True:
        # Check if ".git" is inside any of the files in the current directory.
        # If yes, return the pathname with .git appended at the end. If not,
        # then check if we are at the root. If we are, then break and exit with
        # error, if not, then move to the parent directory and reset path_name
        if ".git" in os.listdir(path_name):
            return os.path.realpath(os.path.join(path_name, ".git"))
        if os.path.dirname(path_name) == path_name:
            break
        os.chdir("..")
        path_name = os.getcwd()

    # If no .git exists, then exit. Using this function and adding a string to
    # the end is the same as exiting with exit(1)
    sys.exit("No .git repository was found")


# PART 2:
# Calls find_git_repo() to get the .git repo and then search for all
# the branch names inside the repo. They are stored inside ./git/refs/heads
# branches_dict is a dictionary. Keys are the hashes and values are a list
# of branch names
def get_branch_names(branches_dict=dict(), local_branches_list=list()):
    git_path = find_git_repo()
    branches = git_path + "/refs/heads"
    branch_names = []

    # The below statement lets us "walk" down a tree
    # root - the current directory that os.walk() is looking in
    # __subdirs - the subdirectories that exist within current directory (root)
    # files - the files that exist within current directory (root)
    # NOTE: __subdirs is not used in this instance (only there for syntax)
    for (root, __subdirs, files) in os.walk(branches, topdown=True):
        # For each file inside current directory (root)
        for f in files:
            # Get path and remove everything up to ../heads to
            # get the commit name. Add to branch_names
            path_name = os.path.join(root, f)
            if path_name.startswith(branches):
                path_name = path_name[len(branches)+1:]
            branch_names.append(path_name)

    # At this point, branch_names should have ALL the branch names from
    # the entire .git/refs/heads folder. Each name is itself a file that
    # contains a hash value. Get this hash value and update branches_dict
    # with the hash as the key and branch names as they value (in a list)
    for name in branch_names:
        # Gets the hash value stored in the file
        hash_value = open(os.path.join(branches, name), 'r').read().strip('\n')

        # If hash_value is NOT in branches_dict, add it as a key and initialize
        # value as an empty list.
        if hash_value not in branches_dict:
            branches_dict[hash_value] = []

        # Append branch name to the list associated with the hash key
        # Add the hash value to local_branches_list
        branches_dict[hash_value].append(name)
        local_branches_list.append(hash_value)


# PART 3:
# Builds the commit graph (commit_nodes) by looping through all the
# local branches gathered in the previous function. Utilizes the
# CommitNode class to represent nodes appropriately.
# Function based off example in Hints Slides
def build_commit_graph(root_hashes=list(), local_branches_list=list()):
    commit_nodes = dict()  # Key - Commit Hash, Value - CommitNode class
    visited = set()
    stack = local_branches_list
    while stack:
        # Get the next element from stack, store it in commit_hash, and
        # remove it from stack
        commit_hash = stack.pop()

        # If commit is already in visited, skip this current commit_hash
        # and continue on the next iteration. If not, add it to visited
        if commit_hash in visited:
            continue
        visited.add(commit_hash)

        # If the commit we’re on isn’t in commit_nodes, add it to
        # commit_nodes. Set the value equal to a CommitNode class
        if commit_hash not in commit_nodes:
            commit_nodes[commit_hash] = CommitNode(commit_hash)

        # Find commit_hash in the objects folder. The first two digits are used
        # to denote the folder. The last digits are used to denote the file.
        first_two_of_hash = commit_hash[0:2]
        hash_file = commit_hash[2:len(commit_hash)]
        file_to_open = find_git_repo() + "/objects/" + first_two_of_hash + "/" + hash_file

        # If file_to_open is a real path.
        if os.path.exists(file_to_open):
            # Open the file and decompress the data. Decode the data so it's
            # easier to read and parse through. Then split the data by newlines
            # and search for "parent". If "parent" exists, add the hash to the
            # appropriate set: commit_nodes[commit_hash].parents.
            data = open(file_to_open, 'rb').read()
            decompressed_data = zlib.decompress(data).decode()
            for line in decompressed_data.split("\n"):
                if line.startswith("parent "):
                    commit_nodes[commit_hash].parents.add(line[7:])

        # If commit_hash doesn't have any parent commits, add it to root_hashes
        if not commit_nodes[commit_hash].parents:
            root_hashes.append(commit_hash)

        # For each parent of the current CommitNode (if they exist):
        for p in commit_nodes[commit_hash].parents:
            # If p isn't in visited, add it to the end of stack to visit later
            if p not in visited:
                stack.append(p)

            # If p isn't in commit_nodes, then add it to commit nodes. p
            # is a hash, so it's the key. The value is a CommitNode of
            # hash p.
            if p not in commit_nodes:
                commit_nodes[p] = CommitNode(p)

            # Register commit_hash as a child commit of p
            commit_nodes[p].children.add(commit_hash)

    return commit_nodes


# PART 4:
# Topologically orders the commits. Utilizes a temp_stack in addition to
# a real stack in order to appropriately represent entire branches.
# Function based off example in Hints Slides
def get_topo_ordered_commits(commit_dict=dict(), root_hashes=list()):
    order = []
    visited = set()
    temp_stack = []
    stack = sorted(root_hashes)

    # While stack still has things in it
    while stack:
        v = stack.pop()

        # If v was already visited, then continue on the next iteration. If
        # not, add v to visited and move forward in code.
        if v in visited:
            continue
        visited.add(v)

        # If temp_stack is NOT empty
        if temp_stack:
            # While temp_stack[-1] (AKA the top) is not in
            # commit_dict[v].parents, append the temp_stack onto order
            while temp_stack[-1] not in commit_dict[v].parents:
                g = temp_stack.pop()
                order.append(g)
                # If temp_stack is now empty, exit while loop
                if not temp_stack:
                    break

        # Now append the current hash onto temp_stack and then look through
        # all the children of v. Only append if the children have not been
        # visited.
        temp_stack.append(v)
        for c in sorted(commit_dict[v].children):
            # If c has already been visited, skip it
            if c not in visited:
                stack.append(c)

    #  Add the rest of the temp_stack to the order
    while temp_stack:
        add_me = temp_stack.pop()
        order.append(add_me)

    return order


# PART 5:
# Properly prints the ordered commits in the manner described by the
# assignment page. Function pulled directly from Hints Slide
def print_topo_ordered_commits_with_branch_names(commit_dict=dict(), topo_ordered_commits=list(), head_to_branches=dict()):
    # jumped is a 'toggle' variable that tells us when there's been a change
    # in the branch currently traversed.
    jumped = False
    for i in range(len(topo_ordered_commits)):
        commit_hash = topo_ordered_commits[i]

        # If we previously jumped, toggle jumped and output all the children
        # of the current node with a sticky end.
        if jumped:
            jumped = False
            sticky_hash = ' '.join(commit_dict[commit_hash].children)
            print(f'={sticky_hash}')

        # If commit_hash exists in head_to_branches, set branches equal to
        # the sorted list of branch names. Then output the commit_hash and
        # and branch names (if any exists).
        branches = sorted(head_to_branches[commit_hash]) if commit_hash in head_to_branches else []
        print(commit_hash + (' ' + ' '.join(branches) if branches else ''))

        # If the next node in the order is not the parent of the current
        # node, set jumped equal to true and output the parents of the
        # current node with a sticky start.
        if i+1 < len(topo_ordered_commits) and topo_ordered_commits[i+1] not in commit_dict[commit_hash].parents:
            jumped = True
            sticky_hash = ' '.join(commit_dict[commit_hash].parents)
            print(f'{sticky_hash}=\n')


if __name__ == '__main__':
    topo_order_commits()
