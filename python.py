import struct
import random

def generate_large_test(filename, num_ops=100000):
    # HBT_MIN and HBT_MAX from hbt.h
    HBT_MIN = -268435456
    HBT_MAX = 268435455
    
    with open(filename, 'wb') as f:
        keys_in_tree = []
        
        for i in range(num_ops):
            # 80% insertions, 20% deletions to keep the tree large
            is_insertion = random.random() < 0.8
            
            if is_insertion or not keys_in_tree:
                # Generate a random key within range
                key = random.randint(HBT_MIN, HBT_MAX)
                op = b'i'
                keys_in_tree.append(key)
            else:
                # Delete a random key currently in the tree
                idx = random.randrange(len(keys_in_tree))
                key = keys_in_tree.pop(idx)
                op = b'd'
            
            # Pack as 4-byte signed integer (little-endian) and 1-byte char
            # Format: i (int), c (char)
            f.write(struct.pack('<ic', key, op))

    print(f"Created {filename} with {num_ops} operations.")

if __name__ == "__main__":
    generate_large_test("large_ops.b", 100000)