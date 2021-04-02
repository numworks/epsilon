.syntax unified

/* Commentaires Romain :
- faire du C dès que possible
- juste copier les args du PS vers le MP, jumper à la fonction, copier les paramètres du MP vers le PS...
- Quelle quantité ? static_assert sur le somme des args des fonctions SVC
- builtin return address ? Avoir l'info dynamiquement
https://stackoverflow.com/questions/21501222/finding-stack-frame-size
*/
.section .text.svcall_handler_as
.align 2
.thumb
.global svcall_handler_as
svcall_handler_as:
  // Step 1: store the exception value to be able to exit exception
  push {lr}
  // Step 2: extract the svc number in $r2
  /* Load the return value in r2 (to access the instruction "svc SVC_NUMBER")
   * NB: the return value is the $pc that was stored in the stack frame. The
   * stack frame is located on the process stack and the register $pc has an
   * offset of 0x18.
   */
  mrs r2, psp                // r2 = psp
  ldr r2,[r2,#0x18]          // r2 = pc value when the userland executed the SVC
  ldrh r2,[r2,#-2]           // Retrieve the instruction "svc SVC_NUMBER" in r2
  bic r2,r2,#0xFF00          // Extract SVC_NUMBER in r2
  // Step 3: Set r0 as the top of process stack
  mrs r0, psp
  // Step 4: set r1 as the exception return value
  mov r1, lr
  // Step 5: Call svcall_handler(r0 = processStackPointer, r1 = exceptReturn, r2 = svcNumber)
  bl svcall_handler
  // Step 6: Restore lr = exception return
  pop {lr}
  // Step 7: jump back to the SVC caller
  bx lr
.type svcall_handler_as, function
