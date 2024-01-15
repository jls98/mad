input_file = 'mpih_sqr_n.txt'
output_file = 'mpih_sqr_n_asm.txt'

with open(input_file, 'r') as infile, open(output_file, 'w') as outfile:
    for line in infile:
        # Überprüfen, ob die Zeile den gewünschten Assembly-Code enthält
        if ':' in line and '\t' in line:
            # Die Teile der Zeile extrahieren, die den Assembly-Code enthalten
            parts = line.split('\t')
            if len(parts) >= 3:
                assembly_code = parts[2].split('#', 1)[0].strip()
                # Die Ergebniszeile in die Ausgabedatei schreiben
                outfile.write(assembly_code + '\n')

print("Der Assembly-Code wurde in die Datei '{}' geschrieben.".format(output_file))
