from copy import deepcopy
class NumMatrix(object):
    def __init__(self, matrix):
        """
        initialize your data structure here.
        :type matrix: List[List[int]]
        """
        self.matrix = matrix
        self.rowsum = copy.deepcopy(matrix)
        self.colsum = copy.deepcopy(matrix)
        for list in self.rowsum:
            for i in range(1,len(list)):
                list[i]=list[i]+list[i-1]
        for row in range(1, len(matrix)):
            for col in range(0, len(matrix[0])):
                self.colsum[row][col] = self.colsum[row][col]+self.colsum[row-1][col]

    def sumRegion(self, row1, col1, row2, col2):
        """
        sum of elements matrix[(row1,col1)..(row2,col2)], inclusive.
        :type row1: int
        :type col1: int
        :type row2: int
        :type col2: int
        :rtype: int
        """
        sum=0
        if row2-row1<=col2-col1:
            # more colums
            for row in range(row1, row2+1):
                sum = sum+self.rowsum[row][col2]-self.rowsum[row][col1]+self.matrix[row][col1]
        if row2-row1>col2-col1:
            for col in range(col1, col2+1):
                sum=sum+self.colsum[row2][col]-self.colsum[row1][col]+self.matrix[row1][col]
        return sum


# Your NumMatrix object will be instantiated and called as such:
# numMatrix = NumMatrix(matrix)
# numMatrix.sumRegion(0, 1, 2, 3)
# numMatrix.sumRegion(1, 2, 3, 4)
# Submission Result: Accepted