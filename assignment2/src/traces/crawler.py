import numpy as np

def parse_file(file_path):
    arrays = []
    with open(file_path, 'r') as file:
        for line in file:
            # Split each line into a list of integers
            values = list(map(int, line.split()))
            arrays.append(values)
    return arrays
	
def append(file_path, sequence):
    with open(file_path, 'a') as file:
        file.write(sequence + '\n\n')
		
def create_threshold_arrays(arrays, threshold=80):
    threshold_arrays = []
    for array in arrays:
        threshold_array = [1 if value < threshold else 0 for value in array]
        threshold_arrays.append(threshold_array)
    return threshold_arrays
	

def detect_blocks(result):
    seperator_start =0
    seperator_end=0
    for i in range(0, len(result)-1):
        #detect seperating MMM block
        if i<(len(result)-100) and result[i]=='S' and result[i+1]=='M' and result[i+2]=='M'and result[i+3]=='M': seperator_start = i+1		
        if i<(len(result)-100) and result[i]=='M' and result[i+1]=='M' and result[i+2]=='M'and result[i+3]=='S': seperator_end = i+3
    return result[:seperator_start], result[seperator_end:]		

def henning_shacham(input, windowsize =2):
# window size of 2 since in measurements there are two very close MUL operations at the beginning -> MUL to calculate base^2 and base^3: -> 1 and 11 if MUL happens
# still, algorithm should work for different windowsizes - can be estimated from data on runtime, however, not tested and Rule 2 from the paper does not make sense to me yet
    tri_state_bitstring = "0b1"
    unfinished = True
    input_len = len(input)
    input_index=-1 # since current_position is always set at the last position where a MUL op was detected, -1 start index and -1 for zero amount to fill (the M was already proceeded)
    while(unfinished):
        search_next=True 
        current_position = input_index
        
        while(search_next):
            
            if input_index+1 == input_len-1:
                # finish
                search_next=False
                unfinished=False
                #zeros
                if input_index-(windowsize-2) > current_position: tri_state_bitstring+=(input_index-current_position-(windowsize-1))*'0' #XIE
                for w in range (0, windowsize-1):
                    if input[input_index-w]=='S':tri_state_bitstring+='?' # ending SS
                tri_state_bitstring+='?' # Ending SMS -> only last S unclear            
            #find next SM block
            if input[input_index+1]=='M':
                search_next=False
                # zeros CX...XSM-> 0...0?1, C=current_position, S=input_index, M=input_index+1 CSSM
                if input[input_index-windowsize+1]=='S' and input_index: # 0...0?1
                    #put zeros
                    if input_index-(windowsize-1) > current_position: tri_state_bitstring+=(input_index-current_position-windowsize)*'0' # SSSSM/CXXIM -> 00?1
                    tri_state_bitstring+=(windowsize-1)*'?'
                tri_state_bitstring+='1'                    
            input_index+=1   
    return tri_state_bitstring
	# 1st start contains 1
	# 2nd SM blocks contain 1, if its SSM its ?1, if its S+SSM its 0+?1, SMSM is 11
	# 3rd last S is ? (since we dont know if its S or SM)
	
	# start with 1, look for next SM sequence, SM is 1, every S before is ? if within W range, else 0, if S is from last digit, every S is 0 except the last 2 ones are ??

#iterate and apply windows

def find_magic_numbers(N, e=65537):
    candidates = []

    for kp in range(1, e):
        for kq in range(kp, e):
            prod = kp*kq
            if not ((prod -kp- kq+1 - (prod * N))% e): 
                candidates.append((kp, kq))
                print(kp, kq)

    return candidates

def retrieve_dX():
    N = 0xae70c6981aaf9ae504907b09e31ac1f302905f3a85c11b949de518bca1144573089ad4897b11a6b6f09a7e2ab45c25cedbe90a1f1f3af8fe287c24cd5f8e28be20413a06398121e8b4704578cc65406b0e036b09fb3e8355d0918258bc17c5a972440469603c88f872009413bbcc07f87313c8918664a861d217b510f6009b7e1c280b80b41b7d1e94db53928c0c1e83a139d0ba0969f1f3ec5666d73830fa335de5dae20b9d0ad81f9b8dfbc3925535ffcc388063b9002f37eb5951219af34d7735cf99b856ea4c82784796282b2989eb409b0ca439e0a6946639883515fb3a36e1afe540d2a107a37b8bec8e96a5ad89b69a8cabbd2db5978fa867a3fcc6418d8258b06dbdfad0d3e88c7d0d942cd6f64e18ba1215edf5214c065559eb6c99cfe103e72dfe225b6fdf5c87dfecc2860c8ca73503fd44b2ddefe20eb22435d505daa6b8c2d611d5f50a0c57dd0541ed883fb42251fc61a188a81b700eb83d8755f8dd6d5ad3fc07b282bd71067d4413da4b8cfc9d5ec0bde647365e9a964105a56209f3f970559da1008cbb5744e60658190e98aa786cc7531e176f542d98440db2b4e39a0ee8eaeed9090f8e9b69f493d3b56c266c4bf7bff99d13d7f16c46454e19e50cc82d8e940b1e909839c0999348a23d800555e7b252786298f3e261bd2a3ec0a859b43c364121caba1475f77f661143196d9e4e605f3b4c16abd71d
    candidates = find_magic_numbers(N, 65537)
    for kp, kq in candidates:
        print(f"kp: {kp}, kq: {kq}")



def parse_intern(file_path):

    arrays = parse_file(file_path)
    
    threshold=80
    threshold_arrays = create_threshold_arrays(arrays, threshold)
    res =""
	
	# do not count multiple cache hits in a row multiple times
    foundM=0 
    foundS=0
	
	# accidental cache misses should not be counted or result in two cache hits
    local_M_not_found_counter=0 
    local_S_not_found_counter=0
    
    ### task2 ###
    for i in range(0, len(arrays)):
        if arrays[i][0]<threshold: 
            if foundM==0 and local_M_not_found_counter>3:
                res+="M"
                foundM=1
                local_M_not_found_counter=0
        else:
            foundM=0
            local_M_not_found_counter+=1
        if arrays[i][1]<threshold: 
            if foundS==0:
                res+="S"
                foundS=1
                local_S_not_found_counter=0
        else:
            foundS=0 
            local_S_not_found_counter+=1	
    ### task2 ###
# detect start and end sequence of MUL and remove them
# detect first block for d_q and second block for d_p calculation and separate them
    #print("task 2:") # toggle for task 2 solution
    #print(res)
    #detect start and end
    start_sequence_detected = 0
    end_sequene_detected = 0
    index_last_start=0
    index_begin_end=0
    for i in range(0, len(res)-1):
        if i<1000 and res[i] =='M' and res[i+1] == 'M' and res[i+2] == 'S' and not index_last_start: index_last_start=i+2
        if i>(len(res)-1000) and res[i] == 'S' and res[i+1] == 'M' and res[i+2] == 'M': index_begin_end=i+1
    result_cutted= res[index_last_start:index_begin_end] # [a:b)

    # detect blocks
    return detect_blocks(result_cutted)
	

def parse():
    dps=[]
    dqs=[]
    for i in range (1, 50):
        dp_cand, dq_cand = parse_intern("out-{:02d}.txt".format(i))
        dps.append(dp_cand)
        dqs.append(dq_cand)
        #print(dp_cand)
        #print(dq_cand)
    
    
    counterM=0
    counterS=0
    amount_candidates = len(dps)
    absolute_maj = (amount_candidates*2)/3
    longest_candidate_p=0
    longest_candidate_q=0
    for a in dps: 
        if len(a)>longest_candidate_p: longest_candidate_p=len(a)
    for a in dqs: 
        if len(a)>longest_candidate_q: longest_candidate_q=len(a)
    
    dp_candidate="" # TODO
    for i in range (0, longest_candidate_p):
        for j in range(0, amount_candidates):
            current = dps[j]
            # inside boundary of cur cand?
            if not len(current)-1<i:
                if current[i] == 'M':
                    counterM+=1
                elif current[i] == 'S':
                    counterS+=1
        if counterM+counterS<absolute_maj: break # 1/3 is shorter, only relative majority in most cases probably
        
        dp_candidate = dp_candidate+ 'S' if counterS>=(counterM*3) else dp_candidate+'M'
         
        counterM=0
        counterS=0
        
    print(dp_candidate)        
                
    dq_candidate=""
    amount_candidates = len(dqs)
    absolute_maj = (amount_candidates*2)/3
    counterM=0
    counterS=0
    for i in range (0, longest_candidate_q):
        for j in range(0, amount_candidates):
            current = dqs[j]
            # inside boundary of cur cand?
            if not len(current)-1<i:
                if current[i] == 'M':
                    counterM+=1
                elif current[i] == 'S':
                    counterS+=1
        if counterM+counterS<absolute_maj: break # 1/3 is shorter, only relative majority in most cases probably
        dq_candidate = dq_candidate+ 'S' if counterS>=(counterM*3) else dq_candidate+'M'
         
        counterM=0
        counterS=0
    print(dq_candidate)        

def main():
    file_path="out-01.txt"
    parse()
	
if __name__ == "__main__":
    main()