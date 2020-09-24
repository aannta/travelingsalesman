#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "location.h"

#define MAXDIST 999999
#define MAXCITY 1000

enum input_ok {YES, NO};

//FOR INPUT CHECK
//check whether a string looks like a city code i.e. min 1 alphabet
int iscity (char str[])
{
	int city = 0;
	for (int i = 0; i < strlen(str); i++)
	{
		if ( (str[i] >= 'a' && str[i] <= 'z') || (str[i] >= 'A' && str[i] <= 'Z') )
		{
			city = 1;
		}
	}
	return city;
}

//FOR OUTPUT
void printcity(int curri, char citylist[][3])
{
	printf("%c%c%c", citylist[curri][0], citylist[curri][1], citylist[curri][2]);
	return;
}

void printroute(char citylist[][3], int resind[], int lengthresind)
{
	for (int i = 0; i < lengthresind; i++)
	{
		if (i == lengthresind-1) printcity(resind[i], citylist);
		else
		{
			printcity(resind[i], citylist);
			printf(" ");
		}
	}
	return;
}

//print tour in order where the 2nd original city comes up before the penultimate original city
void printorderedroute(char citylist[][3], int resind[], int lengthresind)
{
	int first = -1;
	int second = -1;
	int last = -1;
	int curr = 0;
	int temparr[MAXCITY];
	int temparrres[MAXCITY];

	//first find where the first, second and last city are in resind
	for (int i = 0; i < lengthresind-1; i++)
		if (resind[i] == 0) first = i;
		else if (resind[i] == 1) second = i;
		else if (resind[i] == lengthresind - 2) last = i;
	curr = first;

	for (int i = 0; i < lengthresind-1; i++)
	{
		temparr[i] = resind[curr];
		curr++;
		if (curr == lengthresind-1) curr = 0;
	}
	temparr[lengthresind-1] = 0; //append 1st city to the end

	for (int i = 0; i < lengthresind-1; i++)
	{
		if (temparr[i] == 0) first = i;
		else if (temparr[i] == 1) second = i;
		else if (temparr[i] == lengthresind - 2) last = i;
	}

	if (second < last)
	{
		printroute(citylist, temparr, lengthresind);
	}
	else if (second > last) //reverse list if second > last
	{
		curr = first;
		for (int i = 0; i < lengthresind-1; i++)
		{
			temparrres[i] = temparr[curr];
			curr--;
			if (curr == -1) curr = lengthresind-2;
		}
		temparrres[lengthresind-1] = 0; //append 1st city to the end
		printroute(citylist, temparrres, lengthresind);
	}
	return;
}

//given an index list against ref latlon list, calculate route's total distance
double sumdistance(location latlonlist[], int resind[], int lengthresind)
{
	double result = 0.0;
	for (int i = 0; i < lengthresind-1; i++)
	{
		result += location_distance(&latlonlist[resind[i]], &latlonlist[resind[i+1]]);
	}
	return result;
}


//FOR NEAREST ALGO
//check whether this int is in this index array with this length?
int istraveled(int curri, int resind[], int lengthresind)
{
	int result = 0;
	for (int j = 0; j < lengthresind; j++)
	{
		if (curri == resind[j])
		{
			result = 1;
			break;
		}
	}
	return result;
}

//find the nearest city not already traveled to the current city with index currk
int find_nearest(int currk, int resind[], int lengthresind, location latlonlist[], int citycount)
{
	double curr_dist = 0.0;
	double min_dist = MAXDIST;
	int result = -1;
	for (int i = 0; i < citycount; i++)//cycle through the original list
	{
		if (!istraveled(i, resind, lengthresind))//if the current i-city is not already traveled
		{
			curr_dist = location_distance(&latlonlist[resind[currk]], &latlonlist[i]);
			if (curr_dist < min_dist)
			{
				min_dist = curr_dist;
				result = i;
			}
		}
	}
	return result;
}

//FOR INSERT-NEAREST/FARTHEST ALGOS

//copy from 1 array to another where from and to are true index starting from 0
void copyarray(int org[], int dest[], int from, int to)
{
	int l = 0; //use l to traverse origin
	for (int i = from; i <= to; i++)
	{
		dest[i] = org[l];
		l++;
	}
	return;
}

//return 1 if cityindex is in toured[]
int istoured(int toured[], int tourlength, int cityindex)
{
	int result = 0;
	for (int i = 0; i < tourlength; i++)
	{
		if (cityindex == toured[i])
		{
			result = 1;
			break;
		}
	}
	return result;
}

void createuntoured(int toured[], int untoured[], int resind[], int tourlength, int lengthresind)
{
	int l = 0; //use l to traverse untoured
	for (int i = 0; i < lengthresind; i++)
	{
		if (istoured(toured, tourlength, resind[i])) continue;
		else 
		{
			untoured[l] = resind[i];
			l++;
		}
	}
	//update remaining cells of untoured to -1
	for (int i = l; i < lengthresind - tourlength; i++) untoured[i] = -1;
	return;
}

//given 2 index arrays, find the pair of cities with shortest distance between them
//return the index from indicated arr where the city's index is stored
//returna = 1 to return indexA, 0 to return indexB
//minmax = 0 to minimize, 1 to maximize
int optpaireddist(location latlonlist[], int arrA[], int arrB[], int lengthA, int lengthB, int returna, int minmax)
{
	double curr_distance = 0.0;
	double opt_distance;
	
	int indexA = -1;
	int indexB = -1;

	if (minmax)
	{
		opt_distance = 0.0;
	} else
	{
		opt_distance = MAXDIST;
	}

	for (int i = 0; i < lengthA; i++)
		for (int j = 0; j < lengthB; j++)
		if (arrA[i] != arrB[j]) //same index=same city so only calculate when different
		{
			curr_distance = location_distance(&latlonlist[arrA[i]], &latlonlist[arrB[j]]);
			
			if (!minmax && curr_distance < opt_distance) 
			{
				opt_distance = curr_distance;
				indexA = i; indexB = j;
			} else if (minmax && curr_distance > opt_distance)
			{
				opt_distance = curr_distance;
				indexA = i; indexB = j;
			}
		}
	if (returna) return indexA;
	else return indexB;
}

//insert new city into existing tour where new tour distance is minimized
void inserttoured(location latlonlist[], int arrA[], int arrB[], int lengthA, int bb)
{
	int temp_arrA[MAXCITY];
	int min_arrA[MAXCITY];
	double curr_distance = 0.0;
	double min_distance = MAXDIST;

	for (int i = 0; i < lengthA-1; i++)
	{
		copyarray(arrA, temp_arrA, 0, lengthA-1);

		for (int j = lengthA; j >= i+2; j--)
		{
			temp_arrA[j] = arrA[j-1];
		}
		temp_arrA[i+1] = arrB[bb];
		curr_distance = sumdistance(latlonlist, temp_arrA, lengthA + 1);
		
		if (curr_distance < min_distance)
		{
			min_distance = curr_distance;
			copyarray(temp_arrA, min_arrA, 0, lengthA);
		}
		
	}

	copyarray(min_arrA, arrA, 0, lengthA);

	return;
}




/**************************************MAIN******************************************************/



int main(int argc, char const *argv[])
{
	enum input_ok input_stat = YES; //at any point when invalid input is print out, switch this to NO so algo isn't executed for other valid methods
	char curr_string[50];
	int city_count = 0;
	char city_list[MAXCITY][3]; //alternative to array of strings
	int curr_city_i = 0; //enumerate through city_count
	location latlon_list[MAXCITY];
	int curr_latlon_i = 0; //curr_latlon_i / 2 == curr_city_i

	if (argc <= 1) //check whether there's even any file first and foremost
	{
		fprintf(stderr, "TSP: missing filename\n");
		input_stat = NO;
	} else
	{
		for (int i = 1; i < argc; i++) //skip no. of arguments argv[0], start from i=1
		{
			if (i == 1)
			{
				FILE * ifile = fopen(argv[i], "r");
				if (ifile == NULL)
				{
					fprintf(stderr, "TSP: could not open %s\n", argv[i]);
					input_stat = NO;
					break;
				} else
				{
					fscanf(ifile, "%i\n", &city_count);
					if ( city_count < 2 )
					{
						fprintf(stderr, "TSP: too few cities\n");
						input_stat = NO;
						fclose(ifile);
						break;
					}
					else while ( fscanf(ifile, "%s", curr_string) != EOF)
					{
						if (iscity(curr_string) && curr_city_i < city_count)
						{
							city_list[curr_city_i][0] = curr_string[0];
							city_list[curr_city_i][1] = curr_string[1];
							city_list[curr_city_i][2] = curr_string[2];
							curr_city_i++;
						} else if (iscity(curr_string) && curr_city_i >= city_count)
						{
							input_stat = NO; 
							break;
						} else if (curr_latlon_i % 2 == 0)
						{
							latlon_list[curr_latlon_i/2].lat = atof(curr_string);
							curr_latlon_i++;
						} else
						{
							latlon_list[curr_latlon_i/2].lon = atof(curr_string);
							curr_latlon_i++;
						}
					}
					fclose(ifile);
				}
			} else if ( !strcmp(argv[i], "-insert") && ( i == argc - 1 ) ) //-insert too complicated, check first
			{
				//missing criterion for 'insert', exit immediately
				fprintf(stderr, "TSP: missing criterion\n");
				input_stat = NO;
				break;
			} else if ( !strcmp(argv[i], "-insert") && ( i < argc - 1 ) )
			{
				//do nothing, move on to next argv
				continue;
			} else if ( !strcmp(argv[i-1], "-insert") && strcmp(argv[i], "nearest") && strcmp(argv[i], "farthest") )
			{
				//Invalid criterion following 'insert', exit immediately
				fprintf(stderr, "TSP: invalid criterion %s\n", argv[i]);
				input_stat = NO;
				break;
			} else if ( i > 1 && strcmp(argv[i], "-nearest") && strcmp(argv[i], "-optimal") && !(!strcmp(argv[i-1], "-insert") && !strcmp(argv[i], "nearest")) && !(!strcmp(argv[i-1], "-insert") && !strcmp(argv[i], "farthest")))
			{
				//invalid method, exit immediately
				fprintf(stderr, "TSP: invalid method %s\n", argv[i]);
				input_stat = NO;
				break;
			} 
		}
	}


/*******************************************************************************************************************/
/*******************************************************************************************************************/

	if (input_stat == YES)
	{
		for (int i = 1; i < argc; i++)
		{
			if ( !strcmp(argv[i-1],"-insert") && !strcmp(argv[i], "nearest") )
			{
				/*----------------------------------------------------------------*/
				/*----------------------------------------------------------------*/
				/*                        INSERT NEAREST                          */
				/*----------------------------------------------------------------*/
				/*----------------------------------------------------------------*/
				
				int res_ind[MAXCITY]; //res_ind[a] = b i.e. city_list[b] is at a-th position
				for (int j = 0; j < city_count; j++) res_ind[j] = j;//initialize res_ind array as original route
				res_ind[city_count] = 0; //append 1st city to the end to make the route circular

				int toured[MAXCITY];
				int untoured[MAXCITY];

				copyarray(res_ind, untoured, 0, city_count);//the whole list is untoured

				for (int j = 1; j < city_count; j++)
				{
					int a = -1, b = -1;
					if (j == 1) //assign 1st pair of cities to toured array
					{
						a = optpaireddist(latlon_list, untoured, untoured, city_count+1, city_count+1, 1, 0);
						b = optpaireddist(latlon_list, untoured, untoured, city_count+1, city_count+1, 0, 0);
						
						//update toured array
						toured[0] = a;
						toured[1] = b;
						toured[2] = a;

						//update untoured array
						createuntoured(toured, untoured, res_ind, 3, city_count+1);

						//update res_ind array to toured + untoured
						copyarray(toured, res_ind, 0, 2);
						copyarray(untoured, res_ind, 3, city_count);

					} else
					{
						a = optpaireddist(latlon_list, toured, untoured, j+1, city_count-j, 1, 0);
						b = optpaireddist(latlon_list, toured, untoured, j+1, city_count-j, 0, 0);
											
						//insert b from untoured to toured at index where new tour's distance is minimized
						inserttoured(latlon_list, toured, untoured, j+1, b);
						
						//update untoured array
						createuntoured(toured, untoured, res_ind, j+2, city_count+1);

						//update res_ind array to toured + untoured
						copyarray(toured, res_ind, 0, j+1);
						copyarray(untoured, res_ind, j+2, city_count);
					}
				}
				printf("-insert %-8s:%10.2f ", argv[i], sumdistance(latlon_list, res_ind, city_count+1));
				printorderedroute(city_list, res_ind, city_count+1);
				printf("\n");
				continue;
			} else if ( !strcmp(argv[i-1], "-insert") && !strcmp(argv[i], "farthest") )
			{
				/*----------------------------------------------------------------*/
				/*----------------------------------------------------------------*/
				/*                       INSERT FARTHEST                          */
				/*----------------------------------------------------------------*/
				/*----------------------------------------------------------------*/

				int res_ind[MAXCITY]; //res_ind[a] = b i.e. city_list[b] is at a-th position
				for (int j = 0; j < city_count; j++) res_ind[j] = j;//initialize res_ind array as original route
				res_ind[city_count] = 0; //append 1st city to the end to make the route circular

				int toured[MAXCITY];
				int untoured[MAXCITY];

				copyarray(res_ind, untoured, 0, city_count);//the whole list is untoured

				for (int j = 1; j < city_count; j++)
				{
					int a = -1, b = -1;
					if (j == 1) //assign 1st pair of cities to toured array
					{
						a = optpaireddist(latlon_list, untoured, untoured, city_count+1, city_count+1, 1, 1);
						b = optpaireddist(latlon_list, untoured, untoured, city_count+1, city_count+1, 0, 1);
						
						//update toured array
						toured[0] = a;
						toured[1] = b;
						toured[2] = a;

						//update untoured array
						createuntoured(toured, untoured, res_ind, 3, city_count+1);

						//update res_ind array to toured + untoured
						copyarray(toured, res_ind, 0, 2);
						copyarray(untoured, res_ind, 3, city_count);

					} else 
					{
						a = optpaireddist(latlon_list, toured, untoured, j+1, city_count-j, 1, 1);
						b = optpaireddist(latlon_list, toured, untoured, j+1, city_count-j, 0, 1);
												
						//insert b from untoured to toured at index where new tour's distance is minimized
						inserttoured(latlon_list, toured, untoured, j+1, b);
						
						//update untoured array
						createuntoured(toured, untoured, res_ind, j+2, city_count+1);

						//update res_ind array to toured + untoured
						copyarray(toured, res_ind, 0, j+1);
						copyarray(untoured, res_ind, j+2, city_count);
					}
				}
				printf("-insert %-8s:%10.2f ", argv[i], sumdistance(latlon_list, res_ind, city_count+1));
				printorderedroute(city_list, res_ind, city_count+1);
				printf("\n");
				continue;
			} else if ( !strcmp(argv[i], "-nearest") )
			{
				/*----------------------------------------------------------------*/
				/*----------------------------------------------------------------*/
				/*                            NEAREST                             */
				/*----------------------------------------------------------------*/
				/*----------------------------------------------------------------*/
				int res_ind[MAXCITY]; //res_ind[a] = b i.e. city_list[b] is at a-th position
				res_ind[0] = 0; //origin city always has index 0

				for (int k = 0; k < city_count-1; k++)//cycle k through remaining res_ind cells and population res_ind[k+1]
				{
					int nearest_ind = -1;
					nearest_ind = find_nearest(k, res_ind, k+1, latlon_list, city_count);
					res_ind[k+1] = nearest_ind;
				}
				res_ind[city_count] = 0;//append origin city as final cell of res_ind

				printf("%-16s:%10.2f ", argv[i], sumdistance(latlon_list, res_ind, city_count+1));
				printroute(city_list, res_ind, city_count+1);
				printf("\n");

				continue;
			} else if ( !strcmp(argv[i], "-optimal") )
			{
				/*----------------------------------------------------------------*/
				/*----------------------------------------------------------------*/
				/*                            OPTIMAL                             */
				/*----------------------------------------------------------------*/
				/*----------------------------------------------------------------*/
				//output original city list + distance according to original route
				int res_ind[MAXCITY]; //res_ind[a] = b i.e. city_list[b] is at a-th position
				res_ind[0] = 0; //origin city always has index 0

				for (int k = 0; k < city_count-1; k++)//cycle k through remaining res_ind cells and population res_ind[k+1]
				{
					res_ind[k+1] = k+1;
				}
				res_ind[city_count] = 0;//append origin city as final cell of res_ind

				printf("%-16s:%10.2f ", argv[i], sumdistance(latlon_list, res_ind, city_count+1));
				printroute(city_list, res_ind, city_count+1);
				printf("\n");

				continue;
			}
		}
	}
	return 0;
}