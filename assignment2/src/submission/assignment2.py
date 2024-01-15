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

    for kp in range(1, e-1):
        try:
            kq = ((1-kp)*pow(kp*N-kp+1, -1, e)) % e
            candidates.append((kp, kq))
        except:
            print("")
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
    
    candidate_dp="000?1" # since LSB(e)*LSB(dp) = LSB(kp)*(LSB(p)-1)+1 and LSB(p)=LSB(e)=1 -> 1*LSB(dp) = LSB(kp)*(1-1)+1 = 1 -> LSB(kp)=1
    candidate_dq="100?1" # since LSB(e)*LSB(dp) = LSB(kp)*(LSB(p)-1)+1 and LSB(p)=LSB(e)=1 -> 1*LSB(dp) = LSB(kp)*(1-1)+1 = 1 -> LSB(kp)=1
    candidate_p="????1" # lsb of primes is 1
    candidate_q="????1" # lsb of primes is 1
    
    unfinished=True
    index_dp_current = index_LSB_dp = len(dp_input)-6
    index_dq_current = index_LSB_dq = len(dq_input)-6
    # len of p, and q unknown
    index_count=5

    candidate_pairs_k = brute_force_k_pairs(N, e)
    candidates_kp = []
    for pair in candidate_pairs_k:  # append all and since they are not doubled look up later and take other part of pair respectively
        candidates_kp.append(pair[0])
    kp_confirmed = []    
    print("start main loop...")
    while(unfinished):
        print("iteration", candidate_p)
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
        
        #print("candidates_p", candidates_p, candidate_p)
        #print("candidates_q", candidates_q, candidate_q)
        #print("candidates_dp", candidates_dp, candidate_dp)
        #print("candidates_dq", candidates_dq, candidate_dq)
        
        # check all combinations and remove which dont have valid combination 
        
        # pq=N index to LSB 
        valid_dp_combination = [] # find all combinations of current candidates with bitlen of current index from LSB
              
        for p in candidates_p: 
            for q in candidates_q: 
                if n_bits(p*q, index_count) == n_bits(N, index_count): # if pq==N do
                    for dp in candidates_dp:    # check candidates for dp
                        for kp in candidates_kp:# and kp  
                            if n_bits((e * dp), index_count) == n_bits(kp*(p-1)+1, index_count): # checking e*dp equation
                                valid_dp_combination.append((p, q, dp, kp))
                                break
        print(valid_dp_combination)    
               
        # check dq
        bit_retrieved = False
        for p, q, dp, kp in valid_dp_combination:
            for dq in candidates_dq:
                # get respective kq 
                for pair in candidate_pairs_k:
                    if pair[0] == kp: 
                        kq = pair[1]
                        break

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
                    print("success")
                    
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
    
    #N = 0xae70c6981aaf9ae504907b09e31ac1f302905f3a85c11b949de518bca1144573089ad4897b11a6b6f09a7e2ab45c25cedbe90a1f1f3af8fe287c24cd5f8e28be20413a06398121e8b4704578cc65406b0e036b09fb3e8355d0918258bc17c5a972440469603c88f872009413bbcc07f87313c8918664a861d217b510f6009b7e1c280b80b41b7d1e94db53928c0c1e83a139d0ba0969f1f3ec5666d73830fa335de5dae20b9d0ad81f9b8dfbc3925535ffcc388063b9002f37eb5951219af34d7735cf99b856ea4c82784796282b2989eb409b0ca439e0a6946639883515fb3a36e1afe540d2a107a37b8bec8e96a5ad89b69a8cabbd2db5978fa867a3fcc6418d8258b06dbdfad0d3e88c7d0d942cd6f64e18ba1215edf5214c065559eb6c99cfe103e72dfe225b6fdf5c87dfecc2860c8ca73503fd44b2ddefe20eb22435d505daa6b8c2d611d5f50a0c57dd0541ed883fb42251fc61a188a81b700eb83d8755f8dd6d5ad3fc07b282bd71067d4413da4b8cfc9d5ec0bde647365e9a964105a56209f3f970559da1008cbb5744e60658190e98aa786cc7531e176f542d98440db2b4e39a0ee8eaeed9090f8e9b69f493d3b56c266c4bf7bff99d13d7f16c46454e19e50cc82d8e940b1e909839c0999348a23d800555e7b252786298f3e261bd2a3ec0a859b43c364121caba1475f77f661143196d9e4e605f3b4c16abd71d
    e = 65537
    #retrieve_dX(dp_bits, dq_bits, N, e)
    
    # test vectors
    dp_t = "0b10?11?10?110000000?11?1000000?1?10000?100000?10000?100?100?10?110?100?1?1?1?1000000?10?1?100?11?10?1000000?1?10?1000?100?1?110?1000000?100?1000?11000?100?10?11?100?100?10?1000?10?1?100?1?10?10?1?10?1000?10?1?1?1?1?1000?100?1?1?100?100?10?11?10?10?1000?1?10?1?10?1000000?1000?100?1?10?100?1000?1100000?100?1000000?10?110?10?100?10?1?1?1?10?10?10?1000?110?1?10?1?10?1100?110?1?10?100?1?100?100?100?1000000?10?1?110?10?1?1000?1100?10?110?1?1?100?1?110?1100?10?1?10?11?10?10?10?1?110?100?1100?1?1?110?1?100?11?100?1?1?1?1000?1000?100?10?1000?100?1?1?1?1?1?100?1?110000?100?1?1?1000?1?10?10?10?11000?10?1?100?1?100?100?1000?100?100?10?11?10?10000?110?10?11?100?1?10000?10?100?1?1?10?100?10?110?1?1?10?100?100?100?10?100?100?10000?10000?110?1?10?100000?1?110?1000000?10?1?110000?1?10000?10?110?10?1?1?10?1?1?10?1?100?1000?11?100?10?1?1000?1?1?1?10?1?1?10?100?100000?1?11?1?1?1?10?1000?1?1000?1?10?110000?1?1?110?100?100?10?1?10?1000?10?100?10?1?1?1000?10000?1?10?1?11?1000?100000?10?1?11?1?10?1?10?100000?10?1?10?110000?1?10?1?10?1?1?10?10?100?1?10?1000000?100000?1?1?1?1?10?1000?11?100?100?10?1100?1?100?1100?11?1?10?10?1?1?10?10?100?10?10?10000?1?1?110?1?100?1?1?100?1?10?10?1000?100?10?1?10?1?10?10?10?1100000?1?10000?1?10?1?1?11?10?10?10000?1000?1?10?10?10?1?1?11?100?1?100?100000?1?100?1?100?100?110?10?1?10?1?1?1?100000?11?1?10000?1?110?1?1000?1000?1100?100000?1?10?1?10000?1?10?1?100?110?10?10000?1100?1?110?11000000000?10?11?1000?1?100?1?110?1?1?10?100?10?1?10000?1?10?100?1100000?110?1?1?1?1000?1?10?10?100000?11?1?100?11?10?110?100?10?1100?100?10?1?10000?10000?1100?10?100?10000000000?10?1?1000?10?11?10?10?1000?1?10?11?100?10?11?10?10?10?1?1?1?10?1?110?100?10?1100?10000?100000?10000?11?1?10?10?10?1?10?1000?1100?1000000?1?110?1?10?10?1000?10000?10?1100?100?100?1000?10?10?1?110?1?10?100?1?10?100?1?1100?1100000000?1?1?1?1?1?100?11?1?100?100?10?100?1?10?10?10?100?110?11000000?10?1?1000?110000?10000?10?10?1?1?10?100000000?100?10?1?10000?10?10?10000?10?1?1?1100000?100?110?10000?10?11?100?1?1?1000?110000?10?110?1?1?1?1100?10?1?1000?1?1000??"
    dq_t = "0b100?1100?1?1000000?10?1?110?100?10?10?1?1?10?1000?1000?1?10000000?1?10?100?1?100?10000?1?1?1?1?1?10?1?1?100?11?10?10?1?100?1?1?1?1000?1100?10?1?10?1?10?100?1?100?10?1100?100?1?100?1?10?10?100?11?1000?11000?1?100?110?10000?100?110000?10?11000?1?10?100?10?110000000?10?1?100?10?1?1?10000?1?10?1?10?1?11000?1?100?1?100?110?1?10?10?100?1?100?100?1000?1000?10?11?10?100?1?100?1?1?10?100?1?10?1?1?10?10?10?10000?100?110?11?1?1000?11?1?100?10?10?100?1?10?1?10000?1?1?1?1?1?10?1?11?1?100?100?1100?1?1000?100?10?10?100?10?10?10?10?1?1?1?1?10?10?1?1?1000?100?1100?1?1?1?10?1?100?110?10?10?110?10?100?1?10?10000?1?10?10000?1100?10000000?10?10000?1?1?100?1?100?11?10?1?10?110?110?1?1000?1000?11?10?10?1000?1?1?10?1?1?100?100?1?10000?1100?100?110?10?1?10000000?11?10?10?1000?100000?1?1?10000000?1?100?1000?1?110?1?100?1?10?110?100?1?1?10?11000?1?100?1?1?100?1?1000?1?10?110?1?100?10?100?1?110?10000?1?10?1?1?10000?1?10?1?10000?1?11?100?11?1?100?1100?1000?1?11?1?10?1?110000?10?1?10?10?110000?1000?1?1?10000?10?10?10?1000?110?1100000?10?1?10?10?1?10?1?1?100?10?1?1?10000?10?110?10?1?100?1?10?1100?10?1000?1?1?100?1?100000000?110?110000000?1?10000?10000?1?10?11?100?1000?1?1?1?1?1000?10?1?10?10?1?1?1?1?10?10?1000?1?1100?100000?110?10000000000?11?10?100?10?11?10?100?1?1?1?100?1?10?10?1?10?100?10?10?1?10?1?100?10?110?1?1?1100?10000?100?1?100?10?1?1000?10?110?10?100?1?10?11?100?110?10000?1?10?1?1100?1?1?100?1?100?10?1100?1?100?110?100?10?10?10?10?1?10?100?1100?1000?1?100?1100?10?110?1?10?10?10?11000?1000?10?100?1?10?11?10?1?100?1?1?1?110?10?1?10?1100?1?1?11000?1?10?1?10?11000?10?1?110?1000?100?1?1?10000?10?100?10?1?10?10?1?10?1?1?10?1?10?100?1?10?100?1?1000?11000?100000000?11?1?10000?110000?1?1?110?10?10?10?1100?110?11?100?100?1?1?1000?1100?1?110?1?10?100000?10?1100?10?1?100?10?1?11?10?1100?1100?1?100?11?10?1?10?1?10?10?1?1?1?10?1?1000000000?10?100?1?10?110?100?1?110?11?1?1000?11000?1?10?110?10?110?1?10?10?1?1?100?1?100?110?1?110?1?10?10000?1?100000?10?1?10?1?1?100?1?10000?100?1000?10?1?1?100?110000?1000?1000000?1?1?10?100?110?10?100?1?110?100??"
    N = 0xd3791f7d0f27c289171b07c70c478528e2826605ea7114f7e38514071aa3cc8c8e3c2fa6d2088c63c9825d117ebb8788c94590959e7e593f6aace6c47e64e546dff5c9cd9c62d533e2ffe789b3d024e6900dee89ef0590b93b3060bc9cf710b5ed9406e850cec59ae072df9812d755181e3193112388cf47583cf3044d47437cbe6d8482d7662457c4d53bb8d41083d1ea55f1607754e66f0202d1a388253a365bf56d55a0f371ce1d704a7fc4dfebfb642b79a54bc9c9272ee2e0e17ef9e15b445da2c81a8968fc65fa8df12fc44e2046a637dd43c33f08436cd560079d377cdf9280d0e0032e7417057045897194a18d749bffd7574fbb78a9f0ac3f203b313d938be6eadb13dd9618a68f5146caa8f89fab3482a142a682cf1a8458b06feec96451cb83be5718c69a35bb6461884d2adc09381c1075fe8b6ee01bc805fc6f6d6a0434e41476dec7546982ec92fb024446f35b0609b34d4bb59274f211ac3959872f71ee34f55a9930e4add13d5a706804451fa5e0acd2c0bb42211d52772faa191fe456cac803bff88f8ebc77da35d0ddb8e66030caee7d6c15a9217ec7ee44490ccaa74cbe034b669be83edf91ad7798829a09e22ce89f12b7788d17c41f970a57b18dc483fd18875090ecc347cad9c36eddea4b4e407678c51ed3d097a2d3ccaf5bf4947b70244e58b6934a17b3825774e80a479fad7735bcbdb4905ed5
    retrieve_dX(dp_t, dq_t, N, e)

	


def main():
    file_path="out-01.txt" # enter file path here
    parse(file_path)
	
if __name__ == "__main__":
    main()