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

def brute_force_k_pairs(N, e):
    print("calculating kX pairs")
    candidates = []

    for kp in range(1, e):
        prod = kp*(kp-1)
        for kq in range(kp, e):  # Iterate from kp to e, avoiding mirrored combinations
            prod += kp # attem
            diff = (prod - kp - kq +1 - prod*N) % e # = 0 if equal/fitting 

            if not diff:
                candidates.append((kp, kq))
                
    print("finished computing kX pairs")

    return candidates

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
                tri_state_bitstring+='?' # Ending SMS -> only last S unclear (last bit must be 1 since )           
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
# get_x_bits_of_number_binary
def n_bits(number, bits):
    return format(number, f'0{bits}b')[len(format(number, f'0{bits}b'))-bits:]

def check_pqN (p, q, N, bits):
    if n_bits(p*q, bits) != n_bits(N, bits):
        return False
    return True
    
def check_equations(p, q, N, e, dp, dq, bits):
    if n_bits(p*q) != n_bits(N):
        return False

    if e * dp % (p - 1) != 1:
        return False

    if e * dq % (q - 1) != 1:
        return False

    return True

def generate_combinations(s):
    result = []

    def generate_helper(current_combination):
        if '?' not in current_combination:
            result.append(int(current_combination, 2))
            return
        index = current_combination.index('?')
        generate_helper(current_combination[:index] + '0' + current_combination[index+1:])
        generate_helper(current_combination[:index] + '1' + current_combination[index+1:])

    generate_helper(s)
    return result
    

    
def retrieve_dX(dp_input, dq_input, N, e):
    print("trying to recover missing bits of dp and dq...")
    
    candidate_dp="1" # since LSB(e)*LSB(dp) = LSB(kp)*(LSB(p)-1)+1 and LSB(p)=LSB(e)=1 -> 1*LSB(dp) = LSB(kp)*(1-1)+1 = 1 -> LSB(kp)=1
    candidate_dq="1" # since LSB(e)*LSB(dp) = LSB(kp)*(LSB(p)-1)+1 and LSB(p)=LSB(e)=1 -> 1*LSB(dp) = LSB(kp)*(1-1)+1 = 1 -> LSB(kp)=1
    candidate_p="1" # lsb of primes is 1
    candidate_q="1" # lsb of primes is 1
    
    unfinished=True
    index_dp_current = index_LSB_dp = len(dp_input)-1
    index_dq_current = index_LSB_dq = len(dq_input)-1
    # len of p, and q unknown
    index_count=1

    candidate_pairs_k = brute_force_k_pairs(N, e)
    candidates_kp = []
    for pair in candidate_pairs_k:  # append all and since they are not doubled look up later and take other part of pair respectively
        candidates_kp.append(pair[0])
        candidates_kp.append(pair[1])
    kp_confirmed = []    
    print("candidates_kp", candidates_kp)
    print("start main loop...")
    while(unfinished):
        print(candidate_dp, type(candidate_dp))
        index_dp_current-=1 # go up to MSB
        index_dq_current-=1 # go up to MSB
        index_count+=1

        # empty candidate lists for ? symbols
       
        
        # next bit is unknown right now and respectively marked
        candidate_dp = dp_input[index_dp_current] + candidate_dp
        candidate_dq = dq_input[index_dq_current] + candidate_dq
        candidate_p = "?" + candidate_p
        candidate_q = "?" + candidate_q
        
        # create candidate lists, 
        candidates_p = generate_combinations(candidate_p)
        candidates_q = generate_combinations(candidate_q)
        candidates_dp = generate_combinations(candidate_dp)
        candidates_dq = generate_combinations(candidate_dq)
        
        print("candidates_p", candidates_p)
        print("candidates_q", candidates_q)
        print("candidates_dp", candidates_dp)
        print("candidates_dq", candidates_dq)
        
        # check all combinations and remove which dont have valid combination 
        
        # pq=N index to LSB 
        valid_dp_combination = [] # find all combinations of current candidates with bitlen of current index from LSB
        for p in candidates_p: 
            for q in candidates_q: 
                if check_pqN(p, q, N, index_count): # if pq==N do
                    for dp in candidates_dp:    # check candidates for dp
                        for kp in candidates_kp:# and kp
                            if n_bits((e * dp), index_count) == n_bits(kp*(p-1)+1, index_count): # checking e*dp equation
                                valid_dp_combination.append((p, q, dp, kp))
            
               
        # check dq
        bit_retrieved = False
        for p, q, dp, kp in valid_dp_combination:
            for dq in candidates_dq:
                kq = -1
                # get respective kq 
                for pair in candidate_pairs_k:
                    if pair[0] == kp: kq = pair[1]
                    if pair[1] == kp: kq = pair[0]
                if kq==-1: 
                    print("fail, could not find match for kp")
                    return -1
                
                # equation 
                if n_bits((e * dq), index_count) == n_bits(kq*(q-1)+1, index_count):
                    # success
                    candidate_p=bin(p)[2:] # set current p 
                    candidate_q=bin(q)[2:] # set current q 
                    candidate_dp=bin(dp)[2:] # set current p 
                    candidate_dq=bin(dq)[2:] # set current q 
                    if kp not in kp_confirmed:
                        kp_confirmed.append(kp)
                    bit_retrieved = True
                    break
                    
        if not bit_retrieved: print("could not find fitting candidates!!!")
        

 
  

        if not "?" in candidate_p and not "?" in candidate_q and int(candidate_p, 2)*int(candidate_q, 2) == N: unfinished = False
    print("\ndp")
    print(candidate_dp)
    print("\ndq")
    print(candidate_dq)
    print("\np")
    print(candidate_p)
    print("\nq")
    print(candidate_q)


    
    
    

def parse(file_path):

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
    print("task 2, all detected M and S:")
    print(res)
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
    dp_seq, dq_seq = detect_blocks(result_cutted)
    #print("SM sequence without starting and ending MUL blocks:")
    #print(result_cutted)
    print("\ndp:")
    print(dp_seq)
    print("\ndq:")
    print(dq_seq)
    
    dp_bits = henning_shacham(dp_seq)
    dq_bits = henning_shacham(dq_seq)
    
    print("\ndp bits:")
    print(dp_bits)
    print("\ndq bits:")
    print(dq_bits)
    
    N = 0xae70c6981aaf9ae504907b09e31ac1f302905f3a85c11b949de518bca1144573089ad4897b11a6b6f09a7e2ab45c25cedbe90a1f1f3af8fe287c24cd5f8e28be20413a06398121e8b4704578cc65406b0e036b09fb3e8355d0918258bc17c5a972440469603c88f872009413bbcc07f87313c8918664a861d217b510f6009b7e1c280b80b41b7d1e94db53928c0c1e83a139d0ba0969f1f3ec5666d73830fa335de5dae20b9d0ad81f9b8dfbc3925535ffcc388063b9002f37eb5951219af34d7735cf99b856ea4c82784796282b2989eb409b0ca439e0a6946639883515fb3a36e1afe540d2a107a37b8bec8e96a5ad89b69a8cabbd2db5978fa867a3fcc6418d8258b06dbdfad0d3e88c7d0d942cd6f64e18ba1215edf5214c065559eb6c99cfe103e72dfe225b6fdf5c87dfecc2860c8ca73503fd44b2ddefe20eb22435d505daa6b8c2d611d5f50a0c57dd0541ed883fb42251fc61a188a81b700eb83d8755f8dd6d5ad3fc07b282bd71067d4413da4b8cfc9d5ec0bde647365e9a964105a56209f3f970559da1008cbb5744e60658190e98aa786cc7531e176f542d98440db2b4e39a0ee8eaeed9090f8e9b69f493d3b56c266c4bf7bff99d13d7f16c46454e19e50cc82d8e940b1e909839c0999348a23d800555e7b252786298f3e261bd2a3ec0a859b43c364121caba1475f77f661143196d9e4e605f3b4c16abd71d
    e = 65537
    retrieve_dX(dp_bits, dq_bits, N, e)
	

	


def main():
    file_path="out-01.txt" # enter file path here
    parse(file_path)
	
if __name__ == "__main__":
    main()