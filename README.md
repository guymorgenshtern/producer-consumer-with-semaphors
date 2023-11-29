# producer-consumer-with-semaphors

README

Assignment #2: Producer-Consumer Problem

HOW TO RUN:
1. Run included script file
2. Run Producer and Consumer (order doesn't matter as long as run within 5 sec of each other)
3. Exact contents of text.txt will be visible in output.txt
4. If desired, contents of text.txt can be changed. Please make sure to add '%' as the last character of the file and not anywhere else.

DESIGN CONSIDERATIONS:
- .txt files are automatically saved with a newline character in Atom (the editor I used for this assignment). Since this newline character is after the designated end character ('%') the consumer doesn't take it into account. An extra increment was added in Consumer to account for this. If editing text.txt in another editor where the newline is not added, please disregard the increment.
- Each process outputs the total bytes read/written and the Consumer prints contents of the buffer it's currently on
