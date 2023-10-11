#define R 256
#define C 256

void Xor_Images(){	
	unsigned Encrypted_Image[R][C];
	unsigned g[R][C];
	unsigned Z[R][C];
	
	for(int i = 0; i < R; ++i){ 
		for(int j = 0; j < C; ++j){
		
			Encrypted_Image[i][j] = g[i][j] ^ Z[j][i];	
				
		}
	}
	
}
