// homework.cpp : ���ɮץ]�t 'main' �禡�C�{���|��ӳB�}�l����ε�������C
//

//#include "pch.h"
#include <iostream>
#include <cmath>
#include <opencv2/opencv.hpp>
#include <vector>
#include <set>

using namespace cv;
using namespace std;

void rand_vertex(int num, vector<Vec3b>& record);//num�Overtex�ƥ�
void rand_vertex2(int num, vector<Vec3b>& record, Mat img);
void rand_vertex_use_matrixU(int cluster_num, int m, Mat& record_point, vector< vector<double> >& matrix_U, vector<Vec3b>& record_vertex);//�ίx�}U��̪쪺V(V�OCLUSTER���s��)
void new_vertex_and_error(vector<vector<double>>& matrix_U, Mat& record_point, vector<Vec3b>& new_record_vertex, int record_vertex_num, int m);
void fuzzy(vector<vector<double>>& matrix_U, Mat& record_point, vector<Vec3b> record_vertex, int m);
double distance(Vec3b a, Vec3b b);
bool error_diffrence_type1(vector<Vec3b>& record_vertex, vector<Vec3b>& new_record_vertex, double error);
bool error_diffrence_type2(vector<Vec3b>& record_vertex, vector<Vec3b>& new_record_vertex, double error);
void print_map_final(Mat& record_point, vector<vector<double>>& matrix_U, vector<Vec3b>& record_vertex, int n);
void print_matrix_U(vector< vector<double> > matrix_U);
void print_vertex(vector<Vec3b > a);
int test_color_num(Mat img);
Mat result;
//--------------------------------------�imain( )��ơj---------------------------------------
//          �y�z�G����O���ε{�����J�f��ơA�ڭ̪��{���q�o���}�l����
//-----------------------------------------------------------------------------------------------
int main()
{
	int i = 0; //�C�L�Ϥ���
	int cluster_num = 10;                  //�X��cluster
	int m = 3;      //�ۤv�]������
	double error = 150;
	Mat img = imread("17.jpg");
	int map_row = img.rows;                     //�Ϥ�row
	int map_col = img.cols;				     //�Ϥ�col
	int point_num = (img.rows) * (img.cols);                   //�X���I
	imshow("����", img);


	vector< vector<double> > matrix_U(cluster_num, vector<double>(point_num));  //�������ݯx�}
	//vector< vector<int> > ran_color(cluster_num, vector<int>(3));
	vector<Vec3b> record_vertex;       //����vertex
	vector<Vec3b> new_record_vertex;
	rand_vertex(cluster_num, record_vertex);//�H��vertex     �ƫe�u�@
	//rand_vertex2(cluster_num, record_vertex, img);
	//rand_vertex_use_matrixU(cluster_num, m, img, matrix_U, record_vertex);//�M�W����rand_vertex�i�H����

	//print_vertex(record_vertex);
	//fuzzy(matrix_U, img, record_vertex, m);
	//print_map_final(img, matrix_U, record_vertex, i++);

	do
	{
		fuzzy(matrix_U, img, record_vertex, m);
		print_matrix_U(matrix_U);
		print_vertex(record_vertex);
		//print_vertex(record_vertex);
		//print_vertex(new_record_vertex);
		print_map_final(img, matrix_U, record_vertex, i++);
		new_vertex_and_error(matrix_U, img, new_record_vertex, record_vertex.size(), m);//�令�Ƕi�h�j�precord_vertex.size()
		waitKey(500);
	} while (error_diffrence_type2(record_vertex, new_record_vertex, error)); //�o�̥i�H�����O��error��k
	cout << "end" << endl;
	
	cv::imwrite("Result.jpg", result);
	waitKey(0);
	system("pause");
	return 0;
}

void rand_vertex(int num, vector<Vec3b>& record)
{
	cv::RNG rnger(cv::getTickCount());
	Vec3b rand_vertex;

	for (int j = 0; j < num; j++)
	{
		bool same = false;
		rand_vertex[0] = rnger.uniform(0, 255);
		rand_vertex[1] = rnger.uniform(0, 255);
		rand_vertex[2] = rnger.uniform(0, 255);

		for (int i = 0; i < record.size(); i++)
		{
			if (rand_vertex == record.at(i))
			{
				same = true;
				j--;        //�S���W�[vertex  �n��j�馸�ƴ�^�h
				break;
			}
		}
		if (same == false)
			record.push_back(rand_vertex);
	}
}

void rand_vertex2(int num, vector<Vec3b>& record, Mat img)
{
	cv::RNG rnger(cv::getTickCount());
	Vec3b rand_vertex;

	for (int j = 0; j < num; j++)
	{
		bool same = false;
		rand_vertex = img.at<Vec3b>(rnger.uniform(0, img.rows - 1), rnger.uniform(0, img.cols - 1));
		for (int i = 0; i < record.size(); i++)
		{
			if (rand_vertex == record.at(i))
			{
				same = true;
				j--;        //�S���W�[vertex  �n��j�馸�ƴ�^�h
				break;
			}
		}
		if (same == false)
			record.push_back(rand_vertex);
	}
}

void rand_vertex_use_matrixU(int cluster_num, int m, Mat& record_point, vector< vector<double> >& matrix_U, vector<Vec3b>& record_vertex)//�ίx�}U��̪쪺V(V�OCLUSTER���s��)
{
	cv::RNG rnger(cv::getTickCount());
	Vec3b rand_vertex;

	int row = matrix_U.size();
	if (row == 0)
		return;
	int col = matrix_U.at(0).size();

	for (int i = 0; i < col; i++)
	{
		int value = 100; //ı�o���ӭn�i�H�ܰʤ~��
		for (int j = 0; j < row - 1; j++)
		{
			int random_num = 0;//�H���諸��
			random_num = rnger.uniform(0, value);
			matrix_U.at(j).at(i) = (double)random_num / (double)100;
			value -= random_num;
		}
		matrix_U.at(row - 1).at(i) = (double)value / (double)100;
	}
	new_vertex_and_error(matrix_U, record_point, record_vertex, cluster_num, m);//cluster_num���i�H���ΡA�i�H��matrix_U�i�H���
	return;
}

void fuzzy(vector<vector<double>>& matrix_U, Mat& record_point, vector<Vec3b> record_vertex, int m)
{
	int row = matrix_U.size();
	if (row == 0)
		return;
	int col = matrix_U.at(0).size();

	for (int i = 0; i < col; i++)
	{
		bool distance_zero = false;
		for (int j = 0; j < row; j++)          //matrix_U.at(row).at(col)
		{
			double total = 0;
			double upper = distance(record_point.at<Vec3b>(i / record_point.cols, i % record_point.cols), record_vertex.at(j));
			//cout << "��( " << i << " , " << j << " ):";
			for (int k = 0; k < record_vertex.size(); k++)
			{
				if (!isnormal((upper / distance(record_point.at<Vec3b>(i / record_point.cols, i % record_point.cols), record_vertex.at(k)))))//������ӭn�Oif (distance(record_point.at(i), record_vertex.at(k))) == 0);
				{
					for (int l = 0; l < record_vertex.size(); l++)
					{
						if (l == k)
							matrix_U.at(l).at(i) = 1;              //��]�O��X��distance�O0���ܥN��ӱ���F  �����g�{1  ���M�|�X�{ inf  �����~
						else
							matrix_U.at(l).at(i) = 0;
					}
					distance_zero = true;
					//cout << "\npoint:( " << record_point.at(i).first << " , " << record_point.at(i).second << " )\n";
					//cout << "vertex:( " << record_vertex.at(k).first << " , " << record_vertex.at(k).second << " )";
					break;
				}

				total += pow(upper / distance(record_point.at<Vec3b>(i / record_point.cols, i % record_point.cols), record_vertex.at(k)), (double)2 / (m - 1));   //2/(m-1)�w�g�ק�
				//printf("%f || %f , ", (upper / distance(record_point.at(j), record_vertex.at(k))), total);
			}
			if (distance_zero == true)
				break;
			//cout << endl;
			//cout << "---------------------------------------------\n";
			matrix_U.at(j).at(i) = 1 / total;
			/*if (!isnormal(matrix_U.at(i).at(i)))
			{
				cout << "( " << j << " , " << i << " ) is not normal";
				int x;
				cin >> x;
			}*/
		}
		//cout << endl;
	}
}

double distance(Vec3b a, Vec3b  b)
{
	int x = (a[0] - b[0]);
	int y = (a[1] - b[1]);
	int z = a[2] - b[2];
	return sqrt(pow(x, 2) + pow(y, 2) + pow(z, 2));
	//return abs(x) + abs(y) + abs(z);
}

/*void print_matrix_U(vector<vector<double>> &matrix_U)
{
	for (int i = 0; i < matrix_U.size(); i++)
	{
		for (int j = 0; j < matrix_U.at(0).size(); j++)
			printf("%1.5f ,", matrix_U.at(i).at(j));
		cout << endl;
	}
}*/

void new_vertex_and_error(vector<vector<double>>& matrix_U, Mat& record_point, vector<Vec3b>& new_record_vertex, int record_vertex_num, int m)//p394 11.6.8
{
	new_record_vertex.clear();//����e����ƧR��

	for (int i = 0; i < record_vertex_num; i++)
	{
		double color1 = 0;
		double color2 = 0;
		double color3 = 0;
		double total_bottom = 0;
		for (int j = 0; j < record_point.rows; j++)
		{
			for (int k = 0; k < record_point.cols; k++)
			{
				color1 += pow(matrix_U.at(i).at(j * record_point.rows + k), m) * record_point.at<Vec3b>(j, k)[0];
				color2 += pow(matrix_U.at(i).at(j * record_point.rows + k), m) * record_point.at<Vec3b>(j, k)[1];
				color3 += pow(matrix_U.at(i).at(j * record_point.rows + k), m) * record_point.at<Vec3b>(j, k)[2];
				total_bottom += pow(matrix_U.at(i).at(j * record_point.rows + k), m);
			}
		}
		//cout << "before:" << color1 << " , " << color2 << " , " << color3 << " , " << total_bottom << endl;
		color1 = color1 / total_bottom;
		color2 = color2 / total_bottom;
		color3 = color3 / total_bottom;
		//cout << "after:" << color1 << " , " << color2 << " , " << color3 << " , " << total_bottom << endl;
		new_record_vertex.push_back(Vec3b((int)color1, (int)color2, (int)color3));
		//new_record_vertex.push_back(Scalar((int)color1, (int)color2, (int)color3))
		//cout << "new:" << (int)new_record_vertex.back()[0] << " , " << (int)new_record_vertex.back()[1] << " , " << (int)new_record_vertex.back()[2] << " , " << total_bottom << endl;
	}
	return;
}

/*void new_vertex_and_error(vector<vector<double>> &matrix_U, Mat &record_point, vector<Vec3b> &new_record_vertex, int record_vertex_num, int m)//p394 11.6.8
{
	new_record_vertex.clear();//����e����ƧR��

	for (int i = 0; i < record_vertex_num; i++)
	{
		int color1 = 0;
		int color2 = 0;
		int color3 = 0;
		int total_bottom = 0;
		for (int j = 0; j < record_point.rows; j++)
		{
			for (int k = 0; k < record_point.cols; k++)
			{
				color1 += (int)pow(matrix_U.at(i).at(j*record_point.rows + k), m)*(int)record_point.at<Vec3b>(j, k)[0];
				color2 += (int)pow(matrix_U.at(i).at(j*record_point.rows + k), m)*(int)record_point.at<Vec3b>(j, k)[1];
				color3 += (int)pow(matrix_U.at(i).at(j*record_point.rows + k), m)*(int)record_point.at<Vec3b>(j, k)[2];
				total_bottom += (int)pow(matrix_U.at(i).at(j*record_point.rows + k), m);
			}
		}
		//cout << "before:" << color1 << " , " << color2 << " , " << color3 << " , " << total_bottom << endl;
		color1 = color1 / total_bottom;
		color2 = color2 / total_bottom;
		color3 = color3 / total_bottom;
		//cout << "after:" << color1 << " , " << color2 << " , " << color3 << " , " << total_bottom << endl;
		new_record_vertex.push_back(Vec3b((int)color1, (int)color2, (int)color3));
	}
	return;
}*/

bool error_diffrence_type1(vector<Vec3b>& record_vertex, vector<Vec3b>& new_record_vertex, double error)  //�����ժ��~�t�[�_�Ӥj��error�Atrue�N��n���U�@��
{
	double total_error = 0;
	for (int i = 0; i < record_vertex.size(); i++)
	{
		total_error += distance(record_vertex.at(i), new_record_vertex.at(i));
		if (total_error > error)
		{
			cout << "________________________________________________";
			for (int j = 0; j < record_vertex.size(); j++)
			{
				record_vertex.at(j) = new_record_vertex.at(j);
			}
			return true;
		}
	}

	cout << "________________________________________________";
	return false;
}
bool error_diffrence_type2(vector<Vec3b>& record_vertex, vector<Vec3b>& new_record_vertex, double error) //�C�դ�����W�Lerror�N����n�����Atrue�N��n���U�@��
{
	for (int i = 0; i < record_vertex.size(); i++)
	{
		cout << "diffrent:" << distance(record_vertex.at(i), new_record_vertex.at(i));
		if (distance(record_vertex.at(i), new_record_vertex.at(i)) > error)
		{
			cout << "________________________________________________\n";
			for (int j = 0; j < record_vertex.size(); j++)
			{
				record_vertex.at(j) = new_record_vertex.at(j);
			}
			return true;
		}
	}
	cout << "________________________________________________";
	return false;
}

void print_map_final(Mat& record_point, vector<vector<double>>& matrix_U, vector<Vec3b>& record_vertex, int n)//n�O�ĴX���i��
{
	int num = 0;
	int cluster_num = record_vertex.size();
	Mat copy_one(record_point.rows, record_point.cols, CV_8UC3, Scalar((int)255, (int)255, (int)255));

	for (int i = 0; i < copy_one.rows; i++)
	{
		for (int k = 0; k < copy_one.cols; k++)
		{
			int belong_claster = 0;  //�ݩ���@��cluster;
			double max = 0;
			for (int j = 0; j < cluster_num; j++)
			{
				if (max < matrix_U.at(j).at(i * copy_one.rows + k))
				{
					max = matrix_U.at(j).at(i * copy_one.rows + k);//max�n��
					belong_claster = j;
				}
			}
			copy_one.at<Vec3b>(i, k) = record_vertex.at(belong_claster);
			//cout << "[ " << i << " , " << k << "] = " << copy_one.at<Vec3b>(i, k) << endl;
		}
	}
	imshow("rand_map_with_vertex" + to_string(n), copy_one);
	copy_one.copyTo(result);

}

void print_matrix_U(vector< vector<double> > matrix_U)//�L�X�x�}U  P382
{
	int row = matrix_U.size();
	if (row == 0)
		return;
	int col = matrix_U.at(0).size();
	for (int i = 0; i < 100; i++)                    //���ե��Ϋe100��
	{
		cout << "point " << i << ": ";
		for (int j = 0; j < row - 1; j++)
		{
			printf("%1.2f , ", matrix_U.at(j).at(i));
		}
		printf("%1.2f\n", matrix_U.at(row - 1).at(i));
	}
}

void print_vertex(vector<Vec3b > a)
{
	for (int i = 0; i < a.size(); i++)
		cout << a.at(i) << endl;
}

int test_color_num(Mat img)
{
	vector<Vec3b> bucket;
	int num = 0;
	int row = img.rows;
	int col = img.cols;

	for (int i = 0; i < row; i++)
	{
		for (int j = 0; j < col; j++)
		{
			bool same = false;
			for (int k = 0; k < bucket.size(); k++)
			{
				if (img.at<Vec3b>(i, j) == bucket.at(k))
				{
					same = true;
				}
			}
			if (same != true)
			{
				num++;
				bucket.push_back(img.at<Vec3b>(i, j));
			}
		}
	}

	return num;
}