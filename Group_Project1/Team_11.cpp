
#include "STcpClient.h"
#include <stdlib.h>
#include <iostream>
#include <vector>

using namespace std;

int m[24][2]={
	{0,2},{0,3},{1,1},{1,2},{1,3},{1,4},{2,0},{2,1},{2,2},{2,3},{2,4},{2,5},{3,0},{3,1},{3,2},{3,3},{3,4},{3,5},{4,1},{4,2},{4,3},{4,4},{5,2},{5,3}
};

class Board{
public:
	vector<vector<vector<int> > > table;
	Board();
	void print_board();
	vector<Board> get_step(bool is_black);
	vector<vector<int> > get_state();
	
};

Board::Board(){
	vector<int> r;
	vector<vector<int> > c;
	for(int i=0;i<6;i++){
		for(int j=0;j<6;j++){
			for(int k=0;k<6;k++){
				r.push_back(0);
			}
			c.push_back(r);
			r.clear();
		}
		table.push_back(c);
		c.clear();
	}
};

void Board::print_board(){
	for(int i=0;i<6;i++){
		cout<<"layer "<<i<<endl;
		for(int j=0;j<6;j++){
			for(int k=0;k<6;k++){
				cout<<table[i][j][k]<<' ';
			}
			cout<<endl;
		}
		cout<<endl;
	}
};

vector<Board> Board::get_step(bool is_black){
	vector<Board> next_move;
	vector<vector<vector<int> > > cp_table;
	for(int i=0;i<24;i++){
		for(int j=0;j<6;j++){
			if(table[j][m[i][0]][m[i][1]] == 0){
				Board t;
				t.table=table;
				if(is_black) t.table[j][m[i][0]][m[i][1]]=1;
				else t.table[j][m[i][0]][m[i][1]]=2;
				next_move.push_back(t);
				break;
			}
		}
	}
	return next_move;
}

vector<vector<int> > Board::get_state(){
	vector<vector<int> > state;
	vector<int> r;
	bool find;
	for(int i=0;i<6;i++){
		for(int j=0;j<6;j++){
			find=false;
			for(int k=5;k>=0;k--){
				if(table[k][i][j]!=0){
					r.push_back(table[k][i][j]);
					find=true;
					break;
				}
			}
			if(!find) r.push_back(0);
		}
		state.push_back(r);
		r.clear();
	}

	return state;
}


int get_score(vector<vector<int> > state, bool is_black){
	int b_score=0;
	int w_score=0;
	for(int r=1;r<5;r++){
		for(int i=0;i<6-4+1;i++){
			if(state[r][i]==1 && state[r][i+1]==1 && state[r][i+2]==1 && state[r][i+3]==1){
				b_score++;
			}
			if(state[r][i]==2 && state[r][i+1]==2 && state[r][i+2]==2 && state[r][i+3]==2){
				w_score++;
			}
		}
	}

	for(int c=1;c<5;c++){
		for(int i=0;i<6-4+1;i++){
			if(state[i][c]==1 && state[i+1][c]==1 && state[i+2][c]==1 && state[i+3][c]==1){
				b_score++;
			}
			if(state[i][c]==2 && state[i+1][c]==2 && state[i+2][c]==2 && state[i+3][c]==2){
				w_score++;
			}
		}
	}	

	if(state[2][0]==1 && state[3][1]==1 && state[4][2]==1 && state[5][3]==1) b_score++;
	if(state[2][2]==1 && state[3][3]==1 && state[4][4]==1 && state[5][5]==1) b_score++;
	if(state[0][2]==1 && state[1][3]==1 && state[2][4]==1 && state[3][5]==1) b_score++;

	if(state[2][0]==2 && state[3][1]==2 && state[4][2]==2 && state[5][3]==2) w_score++;
	if(state[2][2]==2 && state[3][3]==2 && state[4][4]==2 && state[5][5]==2) w_score++;
	if(state[0][2]==2 && state[1][3]==2 && state[2][4]==2 && state[3][5]==2) w_score++;

	if(state[0][3]==1 && state[1][2]==1 && state[2][1]==1 && state[3][0]==1) b_score++;
	if(state[1][4]==1 && state[2][3]==1 && state[3][2]==1 && state[4][1]==1) b_score++;
	if(state[2][5]==1 && state[3][4]==1 && state[4][3]==1 && state[5][2]==1) b_score++;

	if(state[0][3]==2 && state[1][2]==2 && state[2][1]==2 && state[3][0]==2) w_score++;
	if(state[1][4]==2 && state[2][3]==2 && state[3][2]==2 && state[4][1]==2) w_score++;
	if(state[2][5]==2 && state[3][4]==2 && state[4][3]==2 && state[5][2]==2) w_score++;

	if(state[2][2]==1) b_score++;
	if(state[2][3]==1) b_score++;
	if(state[3][2]==1) b_score++;
	if(state[3][3]==1) b_score++;

	if(state[2][2]==2) w_score++;
	if(state[2][3]==2) w_score++;
	if(state[3][2]==2) w_score++;
	if(state[3][3]==2) w_score++;


	if(is_black) return b_score - w_score;
	else return w_score - b_score;
}

pair<int,int> minmax(Board cur_board, int depth, bool isMax, int h, bool is_black){
	if(depth==h){
		int val;
		vector<vector<int> > s;
		s=cur_board.get_state();
		val=get_score(s, is_black);
		// cout<<"depth: "<<depth<<endl;
		// cout<<"val: "<<val<<endl;
		// for(int i=0;i<s.size();i++){
		// 	for(int j=0;j<s[i].size();j++){
		// 		cout<<s[i][j]<<' ';
		// 	}
		// 	cout<<endl;
		// }
		// cout<<endl;
		return make_pair(val,0);
	}

	if(isMax){
		vector<Board> next_move;
		next_move=cur_board.get_step(is_black);
		vector<int> val_list;
		int v;
		for(int i=0;i<next_move.size();i++){
			v=minmax(next_move[i], depth+1, false, h, is_black).first;
			val_list.push_back(v);
		}
		int val_max=-999;
		int idx=-1;
		for(int i=0;i<val_list.size();i++){
			if(val_list[i]>val_max){
				val_max=val_list[i];
				idx=i;
			} 
		}
		return make_pair(val_max,idx);
	}
	else{
		vector<Board> next_move;
		next_move=cur_board.get_step(is_black);
		vector<int> val_list;
		int v;
		for(int i=0;i<next_move.size();i++){
			v=minmax(next_move[i], depth+1, true, h, is_black).first;
			val_list.push_back(v);
		}
		int val_min=999;
		int idx=-1;
		for(int i=0;i<val_list.size();i++){
			if(val_list[i]<val_min){
				val_min=val_list[i];
				idx=i;
			} 
		}
		return make_pair(val_min,idx);
	}

}


/*
	輪到此程式移動棋子
	board : 棋盤狀態(vector of vector), board[l][i][j] = l layer, i row, j column 棋盤狀態(l, i, j 從 0 開始)
			0 = 空、1 = 黑、2 = 白、-1 = 四個角落
	is_black : True 表示本程式是黑子、False 表示為白子

	return Step
	Step : vector, Step = {r, c}
			r, c 表示要下棋子的座標位置 (row, column) (zero-base)
*/
vector<int> GetStep(vector<vector<vector<int> > > &board, bool is_black)
{
	vector<int> step;
	Board cur_board;
	cur_board.table=board;
	vector<Board> n;
	n=cur_board.get_step(is_black);
	pair<int,int> a;
	a=minmax(cur_board, 0, true, 3, is_black);
	//cout<<a.first<<' '<<a.second<<endl;

	vector<vector<vector<int> > > next_board;
	next_board=n[a.second].table;
	for(int i=0;i<6;i++){
		for(int j=0;j<6;j++){
			for(int k=0;k<6;k++){
				if(next_board[i][j][k]!=board[i][j][k]){
					step.push_back(j);
					step.push_back(k);
					return step;
				}
			}
		}
	}

	return step;
}

int main()
{
	int id_package;
	std::vector<std::vector<std::vector<int>>> board;
	std::vector<int> step;

	bool is_black;
	while (true)
	{
		if (GetBoard(id_package, board, is_black))
			break;

		step = GetStep(board, is_black);
		SendStep(id_package, step);
	}
}




