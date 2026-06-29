#!/bin/bash

########################################################################
# Bash CGI Maze Game
########################################################################

DB="maze.db"

########################################################################
# Maze
########################################################################

MAZE=(
"##########"
"#S.......#"
"#.######.#"
"#.#....#.#"
"#.#.##.#.#"
"#.#.##.#.#"
"#....#...#"
"###.###.##"
"#.......E#"
"##########"
)

HEIGHT=${#MAZE[@]}
WIDTH=${#MAZE[0]}

START_X=1
START_Y=1

EXIT_X=8
EXIT_Y=8


########################################################################
# Defaults
########################################################################

MESSAGE=""
MOVES=0


########################################################################
# URL decode
########################################################################

urldecode()
{
    printf '%b' "${1//%/\\x}"
}


########################################################################
# Cookies
########################################################################

get_cookie()
{
    echo "$HTTP_COOKIE" |
    tr ';' '\n' |
    sed 's/^ *//' |
    grep "^$1=" |
    cut -d= -f2
}


PLAYER=$(get_cookie player)


NEW_PLAYER=0


if [ -z "$PLAYER" ]
then
    PLAYER="guest-$(date +%s)-$$"
    NEW_PLAYER=1
fi


########################################################################
# Load player
########################################################################

load_player()
{

X=$START_X
Y=$START_Y
MOVES=0


if [ -f "$DB" ]
then

    while read P PX PY PM
    do

        if [ "$P" = "$PLAYER" ]
        then
            X=$PX
            Y=$PY
            MOVES=$PM
        fi

    done < "$DB"

fi

}


########################################################################
# Save player
########################################################################

save_player()
{

TMP="$DB.tmp.$$"

FOUND=0


if [ -f "$DB" ]
then

while read P PX PY PM
do

    if [ "$P" = "$PLAYER" ]
    then
        printf "%s %s %s %s\n" \
        "$PLAYER" "$X" "$Y" "$MOVES" >> "$TMP"

        FOUND=1

    else

        printf "%s %s %s %s\n" \
        "$P" "$PX" "$PY" "$PM" >> "$TMP"

    fi

done < "$DB"

fi


if [ "$FOUND" -eq 0 ]
then

printf "%s %s %s %s\n" \
"$PLAYER" "$X" "$Y" "$MOVES" >> "$TMP"

fi


mv "$TMP" "$DB"

}


########################################################################
# Reset
########################################################################

reset_game()
{

X=$START_X
Y=$START_Y
MOVES=0

MESSAGE="<p style='color:blue'><b>Game reset</b></p>"

}


########################################################################
# Process movement
########################################################################

move_player()
{

case "$MOVE" in

North)
    NY=$((Y-1))
    NX=$X
;;

South)
    NY=$((Y+1))
    NX=$X
;;

West)
    NX=$((X-1))
    NY=$Y
;;

East)
    NX=$((X+1))
    NY=$Y
;;

*)
    return
;;

esac


CELL="${MAZE[$NY]:$NX:1}"


if [ "$CELL" = "#" ]
then

    MESSAGE="<p style='color:red'><b>Wall!</b></p>"

else

    X=$NX
    Y=$NY
    MOVES=$((MOVES+1))

fi


}


########################################################################
# Read request
########################################################################

MOVE=""


if [ "$REQUEST_METHOD" = "POST" ]
then

    read -N "${CONTENT_LENGTH:-0}" POSTDATA


    MOVE=$(echo "$POSTDATA" |
    tr '&' '\n' |
    grep '^move=' |
    cut -d= -f2)


    MOVE=$(urldecode "$MOVE")


fi


########################################################################
# Game
########################################################################


load_player


case "$MOVE" in

Reset)
    reset_game
;;

North|South|East|West)
    move_player
;;

esac


save_player



########################################################################
# CGI Header
########################################################################

printf "Content-Type: text/html\r\n"

if [ "$NEW_PLAYER" -eq 1 ]
then
    printf "Set-Cookie: player=%s; Path=/\r\n" "$PLAYER"
fi


printf "\r\n"



########################################################################
# HTML
########################################################################


cat <<EOF

<!DOCTYPE html>
<html>

<head>

<title>Bash Maze</title>

<style>

body {
font-family: Arial;
margin:40px;
background:#eee;
}


pre {

font-size:20px;
background:white;
padding:20px;
display:inline-block;

}


button {

width:90px;
height:35px;
margin:3px;

}

</style>


</head>


<body>


<h1>CGI Bash Maze</h1>


<p>
<b>Player:</b> $PLAYER
<br>
<b>Moves:</b> $MOVES
</p>

EOF



printf "%s\n" "$MESSAGE"



########################################################################
# Draw maze
########################################################################


printf "<pre>\n"


for ((r=0;r<HEIGHT;r++))
do

LINE="${MAZE[$r]}"
OUT=""


for ((c=0;c<WIDTH;c++))
do

CHAR="${LINE:$c:1}"


if [ "$r" -eq "$Y" ] &&
   [ "$c" -eq "$X" ]
then

OUT="${OUT}@"

else

case "$CHAR" in

S)
OUT="${OUT}."
;;

E)
OUT="${OUT}E"
;;

*)
OUT="${OUT}$CHAR"
;;

esac

fi


done


printf "%s\n" "$OUT"


done


printf "</pre>\n"



########################################################################
# Win
########################################################################


if [ "$X" -eq "$EXIT_X" ] &&
   [ "$Y" -eq "$EXIT_Y" ]
then


cat <<EOF

<h2 style="color:green">
🎉 You escaped!
</h2>


<form method="POST">

<button name="move" value="Reset">
Restart
</button>

</form>

EOF


else


cat <<EOF


<form method="POST">

<button name="move" value="North">
North
</button>

<br>


<button name="move" value="West">
West
</button>


<button name="move" value="South">
South
</button>


<button name="move" value="East">
East
</button>


</form>


<form method="POST">

<button name="move" value="Reset">
Reset
</button>

</form>


EOF


fi



cat <<EOF


<hr>

<p>
# = wall<br>
@ = player<br>
E = exit
</p>


</body>
</html>

EOF