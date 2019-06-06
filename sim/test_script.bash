#!/bin/bash

test_num=0

echo "Remember to do these two commands before running this script:
export COLUMNS
export LINES
"
mid_x=$COLUMNS/2
mid_y=$LINES/2

#==============================================================================
#	Commands available in the Simulation
#==============================================================================

function return_to_base () {
	printf "b "
}

function drop_dust () {
	printf "d${1},${2} "
}

function up () {
	for (( i = 0; i < $1; i++ ))
	do
		printf "i"
	done
}

function left () {
	for (( i = 0; i < $1; i++ ))
	do
		printf "j"
	done
}

function down () {
	for (( i = 0; i < $1; i++ ))
	do
		printf "k"
	done
}

function right () {
	for (( i = 0; i < $1; i++ ))
	do
		printf "l"
	done
}

function set_delay () {
	printf "m${1} "
}

function set_timeout () {
	printf "o${1} "
}

function pause_resume () {
	printf "p "
}

function quit () {
	printf "q"
}

function reset () {
	printf "r "
}

function drop_slime () {
	printf "s${1},${2} "
}

function drop_trash () {
	printf "t${1},${2} "
}

function move_device () {
	printf "v${1},${2},${3} "
}

function add_weight () {
	printf "w${1} "
}

function set_battery () {
	printf "y${1} "
}

function show_help_screen () {
	printf "?"
}

function loop () {
	if (( $# >= 1 )) && (( $1 > 0 )) 
	then
		for (( i = 0; i < $1; i++ ))
		do
			printf "!"
		done
	else
		printf "!"
	fi
}

function enter () {
	printf "${1} "
}

function setup_rubbish () {
	enter ${1}
	enter ${2}
	enter ${3}
	printf ""
}

#========================================================================
#	Run a single test.
#========================================================================	

function run_test () {
	(( test_num ++ ))
	echo "Test    : ${test_num}"
	echo "Category: ${category}"
	echo "Details : ${details}"
	echo "Expect  : ${expect}"
	echo ""
		
	sleep 3
	echo ${cmd} | ./a1_n9576321
}

#==============================================================================
category="1, 2, 3, 4, 5. Terminal setup, Status display, Command input area, Border lines, Robotic vacuum"
details="Sets up the terminal and status display"
expect="Status display contains student number, robot direction, battery life, time elapsed, robot load weight and rubbish available.
Command input area is empty.The terminal window is outlined by a border and various parts 
of the display are separated by horizontal and vertical lines.
The device is a distinctive icon 9x9 characters in size. 
The icon is approximately circular in shape.
The outline of the icon is rendered with @ symbols.
The device is stationary at the start.
The icon is placed in the middle of the room"

cmd=$(
	setup_rubbish 0 0 0
	set_timeout 5
)

run_test

#==============================================================================
category="6. Default device behaviour"
details="Tests the deault robot collision, pause and movement behaviour"
expect="After pressing 'p' the robot will move downwards displayed as 90 degrees
Whenever the device hits a wall or charging station, it will pause for one time slice and change direction
The device will never cross or intersect the border
When the 'p' command is encountered, it will pause and resume based on it's current state
The device will never move more than 1 character in any direction"

cmd=$(
	setup_rubbish 50 5 5
	pause_resume
	loop 500
	pause_resume
	loop 100
	quit
)

run_test

#==============================================================================
category="7. Battery life"
details="Battery decreasing from 100% at 1% per second"
expect="The battery initially starts at 100% and decreases at 1% per second"

cmd=$(
	setup_rubbish 0 0 0
	pause_resume
	set_timeout 5
)

run_test

#==============================================================================
category="10. Battery fully discharged"
details="Sets the battery level to 2% to observe behaviour of device at low battery and at 0%"
expect="Robot enters return to base mode and moves towards base. Battery dies and a simulation over
message displays with an option to either quit or reset the simulation.
The existing images in the room should still be visible. The status indicators should be frozen in their state"

cmd=$(
	setup_rubbish 0 0 0
	set_delay 40
	pause_resume
	set_battery 2
	set_timeout 7
)

run_test

#==============================================================================
category="8. Charging station"
details="Show charging station behaving as a wall"
expect="The charging station is 9x3 '#' characters.
The charging station is positioned in the middle of the northern edge of the room, 
immediately below the top border.
When the device is not in return to base mode then the charging station should be treated as a wall."

cmd=$(
	setup_rubbish 0 0 0
	move_device 10 13 0
	pause_resume
	loop 500
	quit
)

run_test

#==============================================================================
category="9. Return to base"
details="Activate return to base mode to observe the device's behaviour in this mode"
expect="The device does not collect rubbish while in return to base mode
If the device collides with the station, it will dock adjacent to the station
While the device is docked, the word 'Docked' displays in the command input area,
Charge accumulates to 100% over 3 seconds
The device remains docked until it's fully charged
When the device is charged, it leaves the dock using it's default movement regime"

cmd=$(
	setup_rubbish 1000 10 5
	pause_resume
	return_to_base
	set_timeout 10
)

run_test

#==============================================================================
category="11. Load carrying capacity"
details="Increase the load to determine functionality at key weights"
expect="When the load is greater than 45g, the device will enter return to base
mode and will stop picking up rubbish that it passes over"

cmd=$(
	setup_rubbish 1000 0 0
	pause_resume
	add_weight 42
	set_timeout 10
)

run_test

#==============================================================================
category="12. Rubbish"
details="Display all rubbish types"
expect="3 different types of rubbish. 1000 pieces of dust, 10 pieces of slime
and 5 pieces of trash. None of the rubbish will collide with any other
piece of rubbish, the device or the charging station and will not spawn
outside the border constraints"

cmd=$(
	setup_rubbish 1000 10 5
	set_timeout 5
)

run_test

#==============================================================================
category="13. Rubbish Collection"
details="Device picking up rubbish using it's pixel collision detection"
expect="Rubbish disappears from floor immediately upon pixel collision with the device.
The weight of the device updates in the status display. 1g increase when it picks up dust,
5g when it picks up slime and 20g when it picks up trash. The available rubbish indicator updates
to show the rubbish is removed."

dust_x=$(( mid_x ))
dust_y=$(( mid_y + 10))
slime_x=$(( mid_x))
slime_y=$(( mid_y + 15))
trash_x=$(( mid_x)) 
trash_y=$(( mid_y + 20))

cmd=$(
	setup_rubbish 0 0 0
	echo "d${dust_x},${dust_y}"
	echo "s${slime_x},${slime_y}"
	echo "t${trash_x},${trash_y}"
	pause_resume
	set_timeout 5
)

run_test

#==============================================================================
category="14. Collision detection"
details="Display pixel collision implementation by spawning dust in a corner and try to pick it up with vacuum"
expect="Dust is unable to be collected due to pixel collision implementation"

cmd=$(
	setup_rubbish 0 0 0
	drop_dust 1 7
	move_device 40 12 180
	pause_resume
	set_timeout 5
)

run_test

#==============================================================================
category="15(i). Simulation controle"
details="Using 'd', 's' and 't' commands to drop rubbish"
expect="Screen initially appears with no rubbish. Dust is spawned first immediately below robot,
then slime is spawned below the dust and the trash is spawned below the slime"

cmd=$(
	setup_rubbish 0 0 0
	loop 100
	echo "d${dust_x},${dust_y}"
	loop 100
	echo "s${slime_x},${slime_y}"
	loop 100
	echo "t${trash_x},${trash_y}"
	loop 100
	quit
)

run_test

#==============================================================================
category="15(ii). Simulation controls"
details="Using 'i', 'j', 'k' and 'l' commands to manually move the device"
expect="Manually move and display pixel collision with charging station and the walls."

cmd=$(
	setup_rubbish 0 0 0
	set_delay 30
	up 50
	left 7
	up 5
	loop 10
	left 30
	quit
)

run_test

#==============================================================================
category="15(ii). Simulation controls"
details="Using 'i', 'j', 'k' and 'l' commands to show return to base is calculated dynamically after being pushed"
expect="Robot enters return to base mode. It is moved manually but still returns to base accurately
after being pushed."

cmd=$(
	setup_rubbish 0 0 0
	set_delay 30
	pause_resume
	return_to_base
	loop 10
	left 30
	set_timeout 10
)

run_test

#==============================================================================
category="15(iii). Simulation controls"
details="Set a different delay period for the main event loop"
expect="Initially movements happen very quickly, then the delay is set to 100ms"

cmd=$(
	setup_rubbish 0 0 0
	pause_resume
	set_delay 10
	loop 200
	set_delay 100
	loop 70
	quit
)

run_test

#==============================================================================
category="15(iv). Simulation controls"
details="Set timeout period command"
expect="The simulation will time out after 5 seconds"

cmd=$(
	setup_rubbish 50 3 1
	pause_resume
	set_timeout 5
)

run_test

#==============================================================================
category="15(v). Simulation controls"
details="Use the quit command"
expect="The simulation will run and then quit, displaying an exit screen"

cmd=$(
	setup_rubbish 0 0 0
	pause_resume
	loop 100
	quit
)

run_test

#==============================================================================
category="15(vi). Simulation controls"
details="Use reset command"
expect="The device moves around for a few seconds and is then reset to initial state"

cmd=$(
	setup_rubbish 0 0 0
	pause_resume
	loop 250
	reset
	loop 100
	quit
)

run_test

#==============================================================================
category="15(vii), 15(viii). Simulation controls"
details="Move the device using the 'v' command and add weight to the device using the 'w' command"
expect="The device starts in the left of the screen and moves right while weight is added"

cmd=$(
	setup_rubbish 0 0 0
	pause_resume
	set_delay 30
	move_device 7 30 0
	loop 100
	add_weight 50
	loop 100
	quit
)

run_test
#==============================================================================
category="15(ix). Simulation controls"
details="Use 'y' command to test behaviour during 25% battery cutoff"
expect="Robot is moving as default until battery crosses below 25% when it starts to return to base"

cmd=$(
	setup_rubbish 0 0 0
	pause_resume
	set_battery 27
	loop 400
	quit
)

run_test

#==============================================================================

category="15(x). Simulation controls"
details="Display the help screen"
expect="The display screen should pop up and there should be nothing else on the screen"

cmd=$(
	setup_rubbish 0 0 0
	show_help_screen
	loop 100
	quit
)

run_test

#==============================================================================








