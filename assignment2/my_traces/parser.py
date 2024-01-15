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

def main():
    file_path = 'gnupg.txt'  # Replace with your actual file path
    arrays = parse_file(file_path)
    
    threshold = 60
    threshold_arrays = create_threshold_arrays(arrays, threshold)
    
    # Print the original arrays
    print("Original Arrays:")
    for array in arrays:
        print(array)
    
    # Print the threshold arrays
    res =""
    scount=0
    for i in range(0, len(arrays)):
        if arrays[i][0]<threshold: 
            res+="S"
            scount+=1
        if arrays[i][1]<threshold: res+="M"
    
    print("res: "+res, len(res))
    
	# new SM string -> ignore sequence before first MMMM and ignore positives in range of 3 data points after last positive
    result_string = ""
    lastSpositive=0
    lastMpositive=0
    
    for i in range(0, len(arrays)):
        if arrays[i][0]<threshold: 
            if lastSpositive>3: result_string+="S"
            lastSpositive=0
        else: lastSpositive+=1
        if arrays[i][1]<threshold: 
            if lastMpositive>3: result_string+="M"
            lastMpositive=0
        else: lastMpositive+=1
    
    print("result_string: "+result_string, len(result_string))
	
	
    # if M and M after ignore
    # if M and S after, 1
    # if S and M after ignore
    # if S and S or nothing after, 0
    key=""
    for i in range(0, len(res)-1):
        if res[i] == "S": 
            if i==len(res)-1 or res[i+1]=="S": key+="0"
        elif res[i] == "M": 
            if i<len(res)-1 and res[i+1]=="S": key+="1"
            
    print("key "+key, len(key), scount)    
        

if __name__ == "__main__":
    main()
