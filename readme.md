# Readme

Publish to git

ΕΚΠΑ - Τμήμα Πληροφορικής και Τηλεπικοινωνιών
Λαζαρίδης Νικόλαος, Μ1485
Ασφάλεια Πληροφοριακών Συστημάτων - Computer Security
Εργασία 2


Explanations n shit are in Greek only atm.
If I get enough requests I may be motivated to translate in English.


If somebody wants access to shellcodes and source zipped files send me a message and I'll send them to you. I have them zipped up with a password otherwise they will be tagged as viruses and deleted (and my account may become flagged).

*!* During Buffer Overflow:
Ο σκοπός μιας Buffer Overflow επίθεσης είναι να αντικαταστήσω τη διεύθυνση επιστροφής της τρωτής συνάρτησης με μία της επιλογής μου, στην οποία έχω τοποθετήσει ASCII κώδικα - shellcode, ο οποίος όταν εκτελεστεί θα μεταβάλλει την κανονική ροή του προγράμματος. Τη στιγμή όπου ένα byte αντιγράφεται στην διεύθυνση επιστροφής, στην οποία έχει αντιγράψει ο καταχωρητής RIP τα περιεχόμενα του, συμβαίνει buffer overflow. Ο IP θα μεταπηδήσει στην νέα διεύθυνση μνήμης από την οποία θα συνεχίσει την εκτέλεση.

*!* What happens when a function in called in C?
A new stack frame has to be created. A stack frame is the memory address range between $ebp and $esp.
The sequence of instructions in the stack frame correspond to the operations function.

$ebp
 |
 |			towards
 |			smaller
 |			memory
 -			addresses
 -			  ||
 -			  --
 _
$esp

function A:
	- push function arguments
	- push space for the return value
	- push return address
	- push fp		; push the frame pointer
	- mov fp, sp	; store the stack pointer in the frame pointer
	- allocate space for locals. Now sp is decremented by one word per local variable
	(jump to the function B)
function B:
	- push space for the return value
	- push the address of the previous stack frame
	- push values of registers that this function uses (so they can be restored)
	- push space for local variables
		do the necessary computation
	- restore the registers
	- restore the previous stack frame
	- store the function result
	- jump to the return address
function A:
	- load the original stack pointer value which rests in the frame pointer
	- pop the old frame pointer
	- pop the return address
	- pop the return value

- Branching utilize PC-relative addressing.
- Jumps utilize absolute (απόλυτη ή ψευδοάμεση) addressing.

Stack Pointer control for inserting, removing values from a register
--------------------------------------------------------------------
> addi $sp, $sp, -4			; Subtract $sp by 4.
> sw $s0, 0($sp)			; Insert value of register to stack (say $t0)
> lw $s0, ($sp) 			; Remove value from the stack and place to the register
> addi $sp, $sp, 4			; add $sp by 4. Repeat these four steps for any local variables you want to extract from the stack.

ShellCode, ASCII codes
======================
Hex code	: x86 Instruction
--------	  ---------------
\x90		: nop
\x40		: inc
\x66		: change operand size
\xe9		: jmp
\xe8		: call



