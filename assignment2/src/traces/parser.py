def append(file_path, sequence):
    with open(file_path, 'a') as file:
        file.write(sequence + '\n\n')


def parse_file(file_path):
    arrays = []
    with open(file_path, 'r') as file:
        for line in file:
            # Split each line into a list of integers
            values = list(map(int, line.split()))
            arrays.append(values)
    return arrays

def create_threshold_arrays(arrays, threshold=80):
    threshold_arrays = []
    for array in arrays:
        threshold_array = [1 if value < threshold else 0 for value in array]
        threshold_arrays.append(threshold_array)
    return threshold_arrays

def parse(file_path):
    #file_path = 'out-01.txt'  # Replace with your actual file path
    arrays = parse_file(file_path)
    
    threshold = 80
    threshold_arrays = create_threshold_arrays(arrays, threshold)
    
    # Print the original arrays
    #print("Original Arrays:")
    #for array in arrays:
    #    print(array)
    
    # Print the threshold arrays
    res =""
    scount=0
    foundM=0
    foundS=0
    foundMcounter=0
    foundScounter=0
    setM =0
    setS =0
    for i in range(0, len(arrays)):
        if arrays[i][0]<threshold: 
            if foundM==0:
                #res+="M"
                #scount+=1
                foundM=1
                foundMcounter+=1
            if foundM and foundMcounter<5:
                foundMcounter+=1
            if foundM and foundMcounter>4 and not setM:
                res+="M"
                setM=1
        else:
            foundM=0
            setM=0
        if arrays[i][1]<threshold: 
            if foundS==0:
                #res+="M"
                #scount+=1
                foundS=1
                foundScounter+=1
            if foundS and foundScounter<5:
                foundScounter+=1
            if foundS and foundScounter>4 and not setS:
                res+="S"
                scount+=1
                setS=1
        else:
            foundS=0
            setS=0
        
        
        # maybe raise amount of positives -> like 4 detections in a row and maybe 4 between each detection
    #for i in range(0, len(arrays)):
    #    if arrays[i][0]<threshold: 
     #       if foundS==0:
     #           res+="M"
     #           scount+=1
    #            foundS=1
     #   else:
     #       foundS=0
      #  if arrays[i][1]<threshold: 
     #       if foundM==0:
     #           res+="S"
     #           foundM=1
     #   else:
     #       foundM=0  
    #
    print("res: "+res + " {}", len(res))
    
    # if M and M after ignore
    # if M and S after, 1
    # if S and M after ignore
    # if S and S or nothing after, 0
    key=""
    
    #detect first sequence of Ms
    found_start =0
    #detect last sequence of Ms
    found_end =0
    for i in range(0, len(res)-1):
        if i<100 and i > 5 and res[i]=="S" and res[i-1]=="M" and res[i-2]=="M" and res[i-3]=="M" and res[i-4]=="M":
            found_start=1
        if i<len(res)-5 and i> len(res)-40 and res[i]=="M" and res[i+1]=="M" and res[i+2]=="M":
            found_end=1
        if found_start and not found_end:
            if res[i] == "S": 
                if i==len(res)-1 or res[i+1]=="S": key+="0"
            elif res[i] == "M": 
                if i<len(res)-1 and res[i+1]=="S": key+="1"
            
    print("key "+key, len(key), scount)    
    append("values/key.txt", key)    
    append("values/sequence.txt", res)    

def main():
    for i in range (1, 51):
        parse("out-"+'{:02d}'.format(i) + ".txt")

if __name__ == "__main__":
    main()
