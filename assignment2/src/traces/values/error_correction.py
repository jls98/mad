from rscode import RSCodec

# Replace this with your 50 bit sequences (as strings of 1s and 0s)
measured_sequences = [...]

# Convert the bit sequences to integers
measured_integers = [int(sequence, 2) for sequence in measured_sequences]

# Determine the number of error-correction bytes based on your requirements
# You might need to adjust these parameters based on your specific use case
total_bits = len(measured_sequences[0])
data_bits = total_bits - (2 * your_desired_ecc_bytes)
ecc_bytes = (total_bits - data_bits) // 2

# Initialize the Reed-Solomon codec
codec = RSCodec(ecc_bytes)

# Encode the bit sequences
encoded_data = codec.encode(measured_integers)

# Simulate errors (you can skip this step if your measurements already contain errors)
import random
num_errors = 5  # You can adjust this based on your requirements
for _ in range(num_errors):
    index = random.randint(0, len(encoded_data) - 1)
    encoded_data[index] ^= 1

# Decode the data to recover the original bit sequence
decoded_data = codec.decode(encoded_data)

# Convert the integers back to bit sequences
recovered_sequences = [format(decoded, 'b').zfill(total_bits) for decoded in decoded_data]

print("Original Bit Sequences:", recovered_sequences)
