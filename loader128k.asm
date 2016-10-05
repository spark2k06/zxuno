                output  loader128k
                
                define  zxuno_port      $fc3b
                define  flash_spi       2
                define  flash_cs        3
                
                macro wreg  dir, dato
                    call    rst28
                    defb    dir, dato
                endm

                org     $4000     ; En la VRAM, aunque el comienzo del codigo sera en $0000

Main            di                ; Deshabilitamos interrupciones

                ld      hl, $0000 ; Copiamos la VRAM al lugar que le corresponde, junto con este mismo codigo
                ld      de, $4000
                ld      bc, $4000
                ldir
                jp      Main2     ; Dejamos libre la zona de la ROM
                
Main2           ld      sp, $4200
                ld      bc, zxuno_port                                           
                out     (c), 0    ; Se selecciona Registro $00 (MASTERMAPPER)
                inc     b         ; Puerto de acceso al registro ($FD3B, basta con incrementar B)
                in      f, (c)
                jp      p, Nonlock                
                halt              ; ROM no rooted, detenemos la ejecucion, dejando la pantalla del snapshot congelada...
                
                
Nonlock         in      a, (c)    ; Cambiamos a modo configuracion (boot)
                or      1
                out     (c), a                                             
                
                dec     b         ; volvemos a seleccionar zxuno_port ($FC3B, basta con decrementar B)
                ld      a, 1
                out     (c), a    ; Se selecciona Registro $01 (MASTERMAPPER) para comenzar la copia entre bancos de la SRAM
                inc     b         ; Puerto de acceso al registro ($FD3B, basta con incrementar B)
                                        
                
                ld      ix, bankorder                
CopyBanks       ld      a,(ix)      
CopyBank02      out     (c),a     ; Ya tenemos acceso al banco origen en $c000                
                exx             
                ld      hl, $c000
                ld      de, $8000
                ld      bc, $4000
                ldir
                exx
                cp      $0b       ; Si se trata del banco 2, lo dejamos en $8000 y se ha terminado toda la copia entre bancos de la SRAM
                jr      z, EndCopy                
                inc     ix
                ld      a,(ix)
                out     (c),a   ; Ya tenemos acceso al banco destino en C000  
                exx                   
                ld      hl, $8000
                ld      de, $c000
                ld      bc, $4000
                ldir 
                exx                
                inc     ix
                cp      $07       ; Al copiar el banco 7, salimos del bucle y dejaremos despues el banco 2 en $8000                
                jr      nz, CopyBanks
                ld      a, $0b                
                jp      CopyBank02

EndCopy         ld      a, $08    ; Parchear con el valor $08 o $09 en funcion de la pagina de ROM que se encuentre paginada en el snapshot (bit 4 del puerto $7ffd)
                out     (c),a
                ld      hl, $0180 ; Leemos por defecto la cuarta ROM de plus3es.rom (64Kb) del primer slot de ROMs desde la SPI flash (Offset parcheable para elegir otra ROM de la flash, pero que sea de BASIC 48K)
                ld      de, $c000 ; Y escribimos en la ROM 0 (Banco 8 de la SRAM) o 1 (Banco 9 de la SRAM) en funcion de la pagina de ROM seleccionada
                ld      a, $40                
                call    rdflsh               
                
                dec     b         ; volvemos a seleccionar zxuno_port ($FC3B, basta con decrementar B)
                out     (c), 0    ; Se selecciona Registro $00 (MASTERMAPPER)
                inc     b         ; Puerto de acceso al registro ($FD3B, basta con incrementar B)
                
                in      a, (c)    ; Cambiamos a modo ejecucion y tambien desrooteamos
                and     $7e
                out     (c),a
                                                                   

                ld      bc, $7ffd ; restauramos registros, etc
                defb    $3e       ; ld a,n (3e n)
port_0x7ffd     defb    $00       ; Aqui parcheamos el byte con la informacion del SNA (byte del puerto $7ffd)
                
                out     (c), a
                
                ld      a,(bordercolor)
                call    $229b     ; Llamamos a la rutina de la ROM correspondiente para cambiar el color
                
                ld      bc,(reg_af_)
                push    bc
                pop     af
                ex      af,af'
                
                ld      bc,(reg_af)  
                push    bc        ; AF lo restauramos al final...                                                            
               
                ld      bc,(reg_bc_) 
                ld      de,(reg_de_)
                ld      hl,(reg_hl_)
                exx

                ld      bc,(reg_bc)
                ld      de,(reg_de)
                ld      hl,(reg_hl)                
                ld      iy,(reg_iy)
                ld      ix,(reg_ix)

                ld      a,(reg_i)
                ld      i,a               
  
                ld      a,(intmode)
                cp      $00
                call    z, im0
                cp      $01
                call    z, im1
                cp      $02
                call    z, im2                        
                
                ld      a,(iff2)    ; IFF2 [Only bit 2 is defined: 1 for EI, 0 for DI] -> SNA Format (http://faqwiki.zxnet.co.uk/wiki/SNA_format)
                bit     2,a
                call    nz,setei                               
                
                ld      a,(reg_r)
                sub     4           ; Restauramos R con su valor original menos 4, ya que las siguientes instrucciones lo elevaran hasta el valor original
                ld      r,a
                
                pop     af
                ld      sp,(reg_sp)
                
                defb    $c3         ; jp nn (c3 nn nn) -> Saltamos al PC del Snapshot
reg_pc          defw    $0000       ; Aqui parcheamos con el valor PC del SNA              
                             
im0             im      0
                ret
im1             im      1
                ret
im2             im      2
                ret
setei           ei
                ret
; ------------------------
; Read from SPI flash
; Parameters:
;   DE: destination address
;   HL: source address without last byte
;    A: number of pages (256 bytes) to read
; ------------------------
rdflsh          ex      af, af'
                xor     a
                push    hl
                wreg    flash_cs, 0     ; activamos spi, enviando un 0
                wreg    flash_spi, 3    ; envio flash_spi un 3, orden de lectura
                pop     hl
                push    hl
                out     (c), h
                out     (c), l
                out     (c), a
                ex      af, af'
                ex      de, hl
                in      f, (c)
rdfls1          ld      e, $20
rdfls2          ini
                inc     b
                ini
                inc     b
                ini
                inc     b
                ini
                inc     b
                ini
                inc     b
                ini
                inc     b
                ini
                inc     b
                ini
                inc     b
                dec     e
                jr      nz, rdfls2
                dec     a
                jr      nz, rdfls1
                wreg    flash_cs, 1
                pop     hl
                ret
        
rst28           ld      bc, zxuno_port + $100
                pop     hl
                outi
                ld      b, (zxuno_port >> 8)+2
                outi
                jp      (hl)
                
bankorder       defb    $09, $00, $0a, $01, $0c, $03, $0d, $04, $0e, $06, $0f, $07

; Aqui parcheamos con los primeros 27 bytes del SNA

reg_i           defb    $00     
reg_hl_         defw    $0000
reg_de_         defw    $0000
reg_bc_         defw    $0000
reg_af_         defw    $0000
reg_hl          defw    $0000
reg_de          defw    $0000
reg_bc          defw    $0000
reg_iy          defw    $0000
reg_ix          defw    $0000
iff2            defb    $00     
reg_r           defb    $00     
reg_af          defw    $0000
reg_sp          defw    $0000   
intmode         defb    $00
bordercolor     defb    $00
