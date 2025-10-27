# README — Proyectos en Assembly

## Descripción
- En esta carpeta se almacenan proyectos escritos en lenguaje ensamblador (Assembly).  
- Objetivo: guardar ejemplos, ejercicios y utilidades en distintos dialectos (NASM, GAS, etc.).

## Estructura recomendada
- src/ — código fuente (.asm, .s).  
- build/ — objetos intermedios (.o).  
- bin/ — ejecutables.  
- docs/ — notas y especificaciones.  
- tests/ — casos de prueba y scripts.  
- examples/ — ejemplos didácticos.

## Convenciones
- Archivos fuente: extensión `.asm` (NASM) o `.s` (GAS).  
- Comentarios claros y encabezado con autor, fecha y objetivo del archivo.  
- Codificación UTF-8.  
- Preferir un Makefile simple para compilar y limpiar.

## Compilación y ejecución (ejemplos)
- NASM (x86_64, Linux):
    - nasm -f elf64 src/main.asm -o build/main.o
    - gcc -no-pie build/main.o -o bin/main
    - ./bin/main
- NASM (x86, 32-bit):
    - nasm -f elf32 src/main.asm -o build/main.o
    - ld -m elf_i386 build/main.o -o bin/main
- GAS (AT&T syntax):
    - gcc -o bin/main src/main.s
- Herramientas útiles: nasm, gcc, ld, objdump, readelf, gdb, strace.

## Buenas prácticas
- Separar lógica en funciones/modulos cuando sea posible.  
- Documentar llamadas a sistema y convenciones de registros.  
- Añadir tests simples para verificar comportamiento (entradas/salidas).

## Cómo contribuir
- Crear una rama nueva por proyecto/feature.  
- Añadir README local si el proyecto tiene instrucciones específicas.  
- Pull request con descripción y pasos para reproducir.

Licencia
- Por defecto usar MIT o la licencia preferida del repositorio.

Contacto
- Incluir en cada proyecto un encabezado con autor y correo (si corresponde).

Notas
- Aclarar el dialecto/arquitectura en cada subcarpeta (ej.: x86_64/nasm).  
- Mantener ejemplos pequeños y autocontenidos para facilitar el aprendizaje.