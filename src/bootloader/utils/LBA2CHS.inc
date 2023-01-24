# *************************************************************************** #
# Convert LBA to CHS                                                          #
#                                                                             #
# abs_sector   = (LBA % SectorsPerTrack) + 1                                  #
# abs_cylinder = (LBA / SectorsPerTrack) / numHeads                           #
# abs_head   = (LBA / SectorsPerTrack) % numHeads                             #
# abs_track  = LBA / (SectorsPerTrack * numHeads)                             #
# --------------------------------------------------------------------------- #
# Parameters:                                                                 #
#   AX = LBA Address to convert                                               #
# Returns:                                                                    #
#   CH = cylinder (low 8 bits)                                                #
#   CL = sector (low 6 bits)                                                  #
#   DH = head number                                                          #
# Destroys:                                                                   #
#   BX, DL, AX                                                                #
# *************************************************************************** #
.ifndef LBA2CHS
.include "filesystems/FAT_bootsector.inc"
.func LBA2CHS
LBA2CHS:
  xor dx, dx            # value is in AX, but division is using DX:AX
  xor cx, cx

  mov bx, _SecPerTrk
  div bx                # DX = LBA % SectorPerTrack
                        # AX = LBA / SectorPerTrack
  inc dx                # adj for sector 0, starting from 1
  mov cl, dl            # storing in CL (abs_sector)

  mov bx, _NumHeads
  xor dx, dx
  div bx                # AX = abs_cylinder
                        # DX = abs_head
  mov ch, al            # low 8 bits of cylinder
  xchg dh, dl           # DH = head number

  # CH cylinder (low 8 bits), CL sector (6 bits), DH head
  ret
.endfunc
.endif
