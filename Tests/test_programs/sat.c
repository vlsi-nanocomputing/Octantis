#define R 16
#define C 16


void sat(){

	int Image[R][C];
	int SAT_Image[R][C];
	int S;

	for(int i = 0; i < R; ++i){
		for(int j = 0; j < C; ++j){
		
			S = 0;
			
			for(int k = 0; k <= i; ++k){
				for(int l = 0; l <= j; ++l){
					SAT_Image[i][j] += Image[k][l];
				}
			}
		      
		}
	}
}



