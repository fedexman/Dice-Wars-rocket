#include "DefaultMap.h"

void LoadDefaultMap(Regions& regions)
{
	regions.clear();
	regions.push_back({ {0,0},{1,0},{2,1},{3,1},{4,2},{4,3},{3,3},{3,2},{2,3},{2,2},{1,2},{1,1},{0,2},{0,1},{4,1},{5,0},{6,0},{4,0},{3,0},{2,0} });
	regions.push_back({ {7,0},{7,1},{7,2},{8,3},{8,4},{8,5},{7,4},{7,3},{6,4},{6,3},{6,2},{6,1},{5,1},{5,2},{5,3} });
	regions.push_back({ {5,4},{6,5},{7,5},{7,6},{7,7},{8,8},{9,8},{8,9},{7,9},{7,8},{6,9},{6,10},{5,9},{5,8},{6,8},{6,7},{6,6},{5,6},{5,7},{4,8},{4,9},{4,7},{4,6},{5,5},{4,5},{3,5},{3,6} });
	regions.push_back({ {3,7},{3,8},{3,9},{4,10},{5,10},{5,11},{4,12},{4,11},{3,11},{3,10},{2,11},{2,10},{2,9},{2,8},{2,7},{1,7},{1,8},{1,9},{1,10},{0,8},{0,7},{1,6},{0,6} });
	regions.push_back({ {1,5},{2,6},{2,5},{3,4},{4,4},{2,4},{1,4},{1,3},{0,4},{0,5},{0,3} });
	regions.push_back({ {0,9},{0,10},{0,11},{1,11},{2,12},{2,13},{3,13},{3,14},{3,15},{2,16},{2,15},{2,14},{1,14},{1,15},{1,13},{1,12},{0,13},{0,14},{0,12} });
	regions.push_back({ {0,15},{0,16},{1,16},{2,17},{3,17},{4,18},{4,19},{3,19},{3,18},{2,19},{2,18},{1,18},{1,17},{0,18},{0,19},{0,17},{4,17},{5,16},{5,15},{4,16},{4,15},{3,16} });
	regions.push_back({ {5,14},{6,15},{7,15},{8,16},{8,15},{7,14},{7,13},{6,14},{6,13},{5,13},{4,14},{7,16},{7,17},{6,18},{6,19},{5,18},{5,17},{6,17},{6,16} });
	regions.push_back({ {5,19},{5,20},{6,21},{6,22},{5,22},{5,21},{4,22},{4,21},{4,20},{3,20},{3,21},{2,22},{2,21},{2,20},{1,20},{1,21},{1,22},{0,22},{0,21},{0,20},{1,19} });
	regions.push_back({ {4,31},{5,31},{6,31},{7,30},{8,30},{7,29},{6,30},{5,30},{5,29},{4,30},{4,29},{4,28},{3,28},{3,29},{3,30},{2,30},{2,29},{2,28},{3,27},{1,28},{1,29} });
	regions.push_back({ {4,27},{5,27},{6,28},{7,28},{8,29},{9,28},{10,28},{9,27},{8,28},{8,27},{7,27},{6,27},{5,26},{4,26},{4,25},{3,25},{3,26},{6,29},{5,28} });
	regions.push_back({ {5,25},{6,26},{7,26},{8,26},{8,25},{8,24},{8,23},{9,22},{10,22},{9,21},{8,22},{8,21},{7,21},{7,22},{7,23},{7,24},{7,25},{6,25},{6,24},{6,23},{5,23},{5,24} });
	regions.push_back({ {7,20},{8,20},{9,20},{10,21},{11,21},{11,20},{12,20},{12,19},{11,19},{11,18},{10,19},{10,20},{9,19},{9,18},{10,18},{9,17},{8,18},{8,19},{7,19},{7,18},{6,20} });
	regions.push_back({ {8,17},{9,16},{10,17},{11,17},{12,18},{13,17},{14,17},{14,16},{13,16},{13,15},{13,14},{13,13},{12,14},{12,15},{12,16},{12,17},{11,16},{11,15},{11,14},{11,13},{12,13},{10,14},{10,15},{10,16},{9,15} });
	regions.push_back({ {9,14},{9,13},{9,12},{10,12},{10,11},{10,10},{9,10},{9,11},{8,12},{8,13},{8,14},{7,12},{7,11},{8,11},{8,10},{9,9},{7,10},{6,11},{6,12} });
	regions.push_back({ {10,9},{11,9},{12,10},{13,9},{13,8},{13,7},{12,8},{12,9},{11,8},{11,7},{12,7},{12,6},{12,5},{11,5},{11,6},{10,7},{10,8},{9,7},{9,6},{10,6},{10,5},{11,4},{9,5},{8,6},{8,7} });
	regions.push_back({ {9,4},{10,4},{11,3},{12,4},{13,4},{14,4},{14,3},{14,2},{13,2},{13,3},{12,3},{12,2},{13,1},{11,2},{11,1},{10,2},{10,3},{9,3},{9,2},{9,1},{10,1},{9,0},{8,1},{8,2},{8,0} });
	regions.push_back({ {10,0},{11,0},{12,1},{13,0},{14,1},{14,0},{12,0} });
	regions.push_back({ {15,0},{16,1},{17,1},{18,1},{19,0},{20,1},{21,1},{21,0},{20,0},{18,0},{17,0},{16,0},{17,2},{16,3},{16,2},{15,2},{15,1} });
	regions.push_back({ {16,5},{17,5},{18,5},{19,4},{20,5},{21,5},{22,6},{23,5},{23,4},{22,5},{22,4},{21,4},{21,3},{20,4},{20,3},{20,2},{19,2},{19,3},{18,4},{18,3},{18,2},{19,1},{17,3},{17,4},{16,4} });
	regions.push_back({ {27,5},{27,6},{27,7},{26,8},{26,7},{26,6},{26,5},{25,5},{25,6},{25,7},{25,8},{24,8},{24,7},{24,6},{24,5},{25,4},{23,6},{23,7},{23,8},{22,8},{22,7} });
	regions.push_back({ {21,7},{21,8},{22,9},{22,10},{23,10},{24,11},{24,12},{23,12},{23,11},{22,12},{22,11},{21,11},{21,10},{21,9},{20,10},{20,9},{20,8},{20,7},{21,6},{19,8},{19,9} });
	regions.push_back({ {19,10},{20,11},{20,12},{20,13},{21,13},{21,14},{21,15},{21,16},{22,17},{22,18},{21,18},{21,17},{20,17},{20,16},{20,15},{20,14},{19,14},{19,15},{19,16},{19,13},{19,12},{19,11},{18,12},{18,13},{18,14},{18,11} });
	regions.push_back({ {15,8},{16,9},{17,8},{18,8},{19,7},{19,6},{20,6},{19,5},{18,6},{18,7},{17,7},{17,6},{16,7},{16,8},{15,7},{15,6},{16,6},{15,5},{14,6},{14,7},{14,8},{13,6},{13,5},{14,5} });
	regions.push_back({ {16,16},{17,16},{18,17},{17,17},{16,18},{16,19},{15,19},{15,20},{14,21},{14,20},{14,19},{15,18},{15,17},{16,17},{15,16},{14,18},{13,18},{13,19},{13,20},{13,21},{12,21} });
	regions.push_back({ {12,22},{13,22},{14,23},{15,22},{15,21},{14,22},{13,23},{12,24},{12,25},{11,25},{11,26},{10,27},{10,26},{10,25},{11,24},{11,23},{12,23},{11,22},{10,23},{10,24},{9,24},{9,25},{9,26},{9,23} });
	regions.push_back({ {11,27},{12,28},{13,28},{14,29},{14,30},{14,31},{13,31},{13,30},{13,29},{12,30},{12,31},{12,29},{11,29},{11,28},{10,29},{14,28},{13,27},{13,26},{13,25},{12,26},{12,27} });
	regions.push_back({ {17,22},{18,23},{19,23},{20,23},{20,22},{20,21},{20,20},{20,19},{20,18},{19,18},{19,19},{19,20},{19,21},{19,22},{18,22},{18,21},{18,20},{18,19},{18,18},{19,17},{17,18},{17,19},{17,20},{17,21},{16,22},{16,21},{16,20} });
	regions.push_back({ {22,19},{23,19},{24,19},{24,18},{25,17},{26,17},{27,16},{27,15},{27,14},{26,15},{26,16},{25,16},{25,15},{25,14},{24,15},{24,16},{24,17},{23,17},{23,18},{23,16},{23,15},{23,14},{24,14},{22,16} });
	regions.push_back({ {24,13},{25,13},{26,14},{27,13},{27,12},{27,11},{27,10},{27,9},{27,8},{26,9},{26,10},{26,11},{26,12},{26,13},{25,12},{25,11},{25,10},{25,9} });
	regions.push_back({ {27,17},{27,18},{27,19},{27,20},{27,21},{27,22},{26,23},{26,22},{26,21},{26,20},{26,19},{26,18},{25,18},{25,19},{25,20},{25,21},{25,22},{24,22},{24,21},{24,20} });
}
