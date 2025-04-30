# Merkle Tree Accumulator

This repository contains a C++ implementation of a **Lattice based Merkle Tree Accumulator**.

## 🔐 Overview

As a part of **ID-based ring signature scheme**, we use the accumulator to hide the signer. Here a group of users—each identified by a unique ID forms a *ring*. A message is signed by one user in the group, but the identity of the actual signer is kept hidden from the verifier. This is achieved through a combination of:

- **Merkle Tree accumulator** for anonymous inclusion verification.
- **Witness generation** to prove a user ID is part of the ring.

## 📌 Key Features

- **C++ implementation** of a Merkle Tree-based accumulator.
- Takes an **n-dimensional vector of elements from 𝑍<sub>q</sub>** as input user IDs.
- Converts IDs to binary strings and hashes them using a cryptographic hash function (here it is a matrix with elements from **Z<sub>q**).
- Accumulates all IDs into a **Merkle root**.
- Provides functionality to:
  - Generate a **witness** for a given ID.
  - **Verify** inclusion of an ID in the Merkle tree (i.e., in the signing ring).
  - Enforce that only users with valid IDs can sign messages.

## 🛠️ Implementation Details

- **Input**: Vector of IDs ∈ 𝑍<sub>q</sub><sup>n</sup>
- **Hashing**: Lattice based hash function
- **Structure**: Binary Merkle Tree for accumulation
- **Language**: C++

## 💡 Use Case

This implementation is also eligible for **post-quantum secure ID-based ring signatures**. It allows:
- Secure anonymous signing within a group
- Efficient ID verification without exposing the signer's identity
- Scalable design suitable for cryptographic protocols and secure messaging systems

