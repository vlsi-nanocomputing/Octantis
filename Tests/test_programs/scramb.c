#define R 16
#define C 16



void XorImage_Key_Generation(){

	//Scrambled images
	int Image_0[R][C], Image_1[R][C], Image_2[R][C], Image_3[R][C], Image_4[R][C], Image_5[R][C];
	//Xor-Keys to be used in decryption
	int Key_0[R][C], Key_1[R][C], Key_2[R][C], Key_3[R][C], Key_4[R][C], Key_5[R][C];
	//Xor Image
	int Xor_Image[R][C];

	//Xor Image generation
	for(int i = 0; i < R; ++i){
		for(int j = 0; j < C; ++j){
			
			//Performing Xor operation on all scrambled images to get the Xor Image
			Xor_Image[i][j] = Image_0[i][j] ^ Image_1[i][j] ^ Image_2[i][j] ^ Image_3[i][j] ^ Image_4[i][j] ^ Image_5[i][j];	
			Key_0[i][j] = Image_1[j][i] ^ Image_2[i][j] ^ Image_3[i][j] ^ Image_4[i][j] ^ Image_5[i][j];
			Key_1[i][j] = Image_0[i][j] ^ Image_2[i][j] ^ Image_3[i][j] ^ Image_4[i][j] ^ Image_5[i][j];
			Key_2[i][j] = Image_0[i][j] ^ Image_1[i][j] ^ Image_3[i][j] ^ Image_4[i][j] ^ Image_5[i][j];
			Key_3[i][j] = Image_0[i][j] ^ Image_1[i][j] ^ Image_2[i][j] ^ Image_4[i][j] ^ Image_5[i][j];
			Key_4[i][j] = Image_0[i][j] ^ Image_1[i][j] ^ Image_2[i][j] ^ Image_3[i][j] ^ Image_5[i][j];
			Key_5[i][j] = Image_0[i][j] ^ Image_1[i][j] ^ Image_2[i][j] ^ Image_3[i][j] ^ Image_4[i][j];
			
		}
	}
}



