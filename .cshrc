#  .cshrc  shell script. This shell script should be sourced by
# every  Karma  user.


# Written by		Richard Gooch	22-SEP-1992

# Last updated by	Richard Gooch	3-NOV-1994


# Alias definitions

# Miscellaneuos aliases
alias 'klint'	'lint -DOS_$OS -DMACHINE_$MACHINE -I$KARMABASE/include -DVERSION=\"date_string\"'
alias 'kxlint'	'lint -DOS_$OS -DMACHINE_$MACHINE -I$KARMABASE/include -I$XINCLUDEPATH -DVERSION=\"date_string\"'
alias 'push_karma'	"clean_karma > /dev/null ; backup -ralwsop /wyvern/karma /applic/karma"
alias 'clean_karma'	'find $KARMAROOT \( -name "*~" -o -name ".*~" \) -print -exec rm {} \;'

if ("$?KARMAROOT" != "0") then
    # Set cd path
    if ("$?cdpath" == "0") then
	set cdpath = ($KARMAROOT)
    else
	set cdpath = ($cdpath $KARMAROOT)
    endif
endif
