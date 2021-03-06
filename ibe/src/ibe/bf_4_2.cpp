/*
 Boneh-Franklin Identity-Based Encryption from the Weil Pairing
 This file is the implementation of BasicIdent scheme of the IBE system.(Chapter 4.2)
 This code needs GMP and PBC library.
 
 Flow Chart:
 (1)Setup:Take secruity parameter K(QBITS,RBITS),return the system parameter ans master
 key of the PKG.The system parameters include a description of a finite message space M,
 and a dscription of a finite ciphertext space C. The system parameters will be publicly
 known,while the master-key will be known only to the PKG.
 (2)Extract:The receiver extracts the corresponding private key from the PKG.
 (3)Encrypt:The sender will generate a ciphertext based on the receiver ID.
 (4)Decrypt:The receiver will use his private key to get the message digest.
 
 Detail:
 1.H1 function---Element build-in function(element_from_hash)
 2.H2 function---SHA1 function generate 160 bit long number
 3.H3 function---Concatenate the sigma and message digest, and
 then put it into build-in function element_random. The random number will between 0 and q.
 4.H4 function---Input a 160 bit long number and run SHA1 function to generate another 160 bit long number.
 5.As I use SHA1 function as H2 function, thus the n is automatically set as 160.
 */


#include "bf_4_2.hpp"

#define SIZE 100
#define RBITS 160
#define QBITS 512


void get_private_key_f(char* ID, pairing_t pairing,element_t s,element_t Sid)
{
  element_t PublicKey,PrivateKey;
  element_init_G1(PublicKey, pairing);
  element_init_G1(PrivateKey, pairing);
  
  element_from_hash(PublicKey, ID, strlen(ID));   //Compute user public key
  element_mul_zn(PrivateKey, PublicKey, s);   //Compute user private key
  Sid=PrivateKey;
  element_printf("Private key Sid = %B\n", Sid);
}

void get_public_key_f(char* ID, pairing_t pairing,element_t Qid)
{
  element_t PublicKey,PrivateKey;
  element_init_G1(PublicKey, pairing);
  element_init_G1(PrivateKey, pairing);
  
  element_from_hash(PublicKey, ID, strlen(ID));   //Compute user public key
  element_printf("\nPublic key Qid = %B\n", PublicKey);
  Qid=PublicKey;
  
  
}


void rand_n(char* sigma)
{
  int i;
  int unit;
  char tempr[10];
  memset(sigma, 0, sizeof(char)*SIZE);//Clear the memory of sigma
  
  for (i = 0; i < 40; i++)
  {
  unit = rand() % 16;
  sprintf(tempr, "%X", unit);
  strcat(sigma, tempr);
  
  }
  
}

void encryption_f(char* shamessage,char* ID, element_t P,element_t Ppub,element_t U,char* V,char* W, pairing_t pairing)
{
  int i;
  char sgid[SIZE];   //Sender gid string representation
  char shagid[SIZE]; //Sender H2 function result
  char sigma[SIZE]; //Sender generate the sigma
  char msigma[2*SIZE]; //Sender concatenate the sigma and message digest
  char ssigma[SIZE]; //It is the result of H4(sigma)
  element_t r;
  element_t Qid;
  element_t gid;
  element_init_G1(Qid, pairing);
  element_init_GT(gid, pairing);
  element_init_Zr(r, pairing);
  rand_n(sigma); //Sender generate a sigma
  strcpy(msigma, sigma);
  strcat(msigma, shamessage);
  element_from_hash(r, msigma, strlen(msigma));
  element_mul_zn(U, P, r);
  element_printf("\nU = %B", U);
  element_pairing(gid, Qid, Ppub);
  element_pow_zn(gid, gid, r);
  element_snprint(sgid, SIZE, gid);
  sha_fun(sgid, shagid); //H2(gid^r)
  sha_fun(sigma, ssigma); //H4(SIGMA)
  
  //Do the XOR operation to the sigma and shagid digest
  for (i = 0; i < 40; i++)
  {
  xor_operation(sigma[i], shagid[i], V);
  }
  //Do the XOR operation to the ssigma and message digest
  for (i = 0; i < 40; i++)
  {
  xor_operation(shamessage[i], ssigma[i], W);
  }
  
  printf("\nV=%s", V);
  printf("\nW=%s\n", W);
}

void decryption_f(element_t Sid,pairing_t pairing,element_t P,element_t U,char* V,char* W,element_t U_receiver,char* shamessage_receiver)
{
  
  int i;
  element_t rgid;
  element_t r_receiver;
  char sgid_receiver[SIZE]; //Receiver calculated gid string representation
  char shagid_receiver[SIZE]; //Receiver H2 function result
  char sigma_receiver[SIZE]; //Receiver compute the sigma
  char ssigma_receiver[SIZE]; //It is the result of H4(sigma_receiver)
  char msigma_receiver[2*SIZE]; //Receiver concatenate the sigma and message digest
  memset(sigma_receiver, 0, sizeof(char)*SIZE);//Clear the memory of sigma_receiver
  memset(shamessage_receiver, 0, sizeof(char)*SIZE);//Clear the memory of shamessage_receiver
  
  element_init_Zr(r_receiver,pairing);
  element_init_GT(rgid, pairing);
  element_pairing(rgid, Sid, U);
  element_snprint(sgid_receiver, SIZE, rgid);
  sha_fun(sgid_receiver, shagid_receiver); //Generate H2(e(dID,U));
  
  //XOR V and H2(e(dID,U))=sigma_receiver
  for (i = 0; i < 40; i++)
  {
  xor_operation(V[i], shagid_receiver[i], sigma_receiver);
  }
  
  sha_fun(sigma_receiver, ssigma_receiver);
  
  //XOR W andH4(sigma)
  for (i = 0; i < 40; i++)
  {
  xor_operation(W[i], ssigma_receiver[i], shamessage_receiver);
  }
  
  strcpy(msigma_receiver, sigma_receiver);
  strcat(msigma_receiver, shamessage_receiver);
  element_from_hash(r_receiver, msigma_receiver, strlen(msigma_receiver));
  element_mul_zn(U_receiver, P, r_receiver);
}

void setup_sys_f(int rbits,int qbits,element_t P,element_t Ppub,pairing_t pairing,element_t s )
{
  
  pbc_param_t par;   //Parameter to generate the pairing
  pbc_param_init_a_gen(par, rbits, qbits); //Initial the parameter for the pairing
  pairing_init_pbc_param(pairing, par);   //Initial the pairing
  
  
  //In our case, the pairing must be symmetric
  if (!pairing_is_symmetric(pairing))
  pbc_die("pairing must be symmetric");
  
  element_init_G1(P, pairing);
  element_init_G1(Ppub, pairing);
  element_init_Zr(s, pairing);
  element_random(P);
  element_random(s);
  element_mul_zn(Ppub, P, s);
  
}