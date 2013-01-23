project_name = 'sht11'
target_elf = project_name + '.elf'
target_hex = project_name + '.hex'
target_up = project_name + '.upload'

vars = Variables('variables.py')
vars.Add('MCU', 'Microcontroller', 'atmega328p')
vars.Add('F_CPU', 'CPU frequency in Hz', 16000000)
vars.Add('BAUD', 'Serial baud rate', 9600)

cdefines = {
	'F_CPU': '${F_CPU}UL',
	'BAUD': '${BAUD}',
}

ccflags = [
	'-mmcu=${MCU}',
	'-Wl,-u,vfprintf',
	'-Os',
	'-lprintf_flt',
	'-lm',
]

elf_build_str = "$CC $CCFLAGS -o ${TARGET} "
for define, value in cdefines.iteritems():
	elf_build_str += "-D" + define + "=" + value + " "
elf_build_str += "${SOURCES}"

elf_build = Builder(
#		action = "$CC $CCFLAGS -o ${TARGET} ${SOURCES}")
		action = elf_build_str)

hex_build = Builder(
		action = "$OBJCOPY -j .text -j .data -O ihex $SOURCE $TARGET")

upload_build = Builder(
		action = "avrdude -P /dev/ttyACM0 -F -c stk500v1 -p m328p -U flash:w:$SOURCE")

builders = {
	'Elf': elf_build,
	'Hex': hex_build,
	'Upload': upload_build,
}

env = Environment(
		variables = vars,
		CCFLAGS = ccflags,
		CDEFINES = cdefines,
		BUILDERS = builders)

env['CC'] = 'avr-gcc'
env['OBJCOPY'] = 'avr-objcopy'

sources = Glob('./*.c')

elf_step = env.Elf(target_elf, sources)
hex_step = env.Hex(target_hex, target_elf)
up_step = env.Upload(target_up, target_hex)
env.Depends(hex_step, target_elf)
env.Depends(up_step, target_hex)

Help(vars.GenerateHelpText(env))

