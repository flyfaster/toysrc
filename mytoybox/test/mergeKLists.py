# Definition for singly-linked list.
class ListNode(object):
    def __init__(self, x):
        self.val = x
        self.next = None
# Submission Result: Accepted
class Solution(object):
    def mergelists(self, left, right):
        if left == right:
            return left
        if left==None:
            return right
        if right==None:
            return left 
        head=None
        node=None
        if left.val <=right.val:
            head = left 
            left=left.next 
        else:
            head=right 
            right=right.next 
        node=head 
        while left!=None and right!=None:
            if left.val<=right.val:
                node.next=left 
                left=left.next 
            else:
                node.next = right 
                right = right.next
            node=node.next
        if left!=None:
            node.next=left 
        if right!=None:
            node.next=right  
        return head
    def mergeKLists(self, lists):
        """
        :type lists: List[ListNode]
        :rtype: ListNode
        """
        if len(lists)==0:
            return None 
#         ret=lists[0]
        for i in range(1, len(lists)):
            k=len(lists)-i
            lists[k/2]=self.mergelists(lists[k/2], lists[k])
        return lists[0]