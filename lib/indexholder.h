/*
=========================================================================================
=========================================================================================
*/
class IndexHolder
{
public:
/*
Constructor. If createInitial is true then an initial index of depth 1 is set up
Poor documentation. Seems that default constructor is to be used in case of intending 
to Load from a previously Written indexholder.
And the constructor with initialDepth should be used when the index does not yet exist
even in a file on disk.
*/
IndexHolder();
IndexHolder(int initialDepth);
/*
Destructor
*/
~IndexHolder();

bool Load(int fileDescriptor);
bool Write(int fileDescriptor);

/*
Add another bit to the index depth. This effectively doubles the size of the index.
The bit is added on the left. The values stored in the index correspond to the old
values, if the new bit were to be ignored.
*/
void IncreaseDepth();

/*
Tests to see if the index can be shrunk. The index can be shrunk iff for all the 
values in the index, that values' "buddy" value has the same value. If so, a bit
is removed from the left, effectively halfing the index size
*/
bool DecreaseDepth();

int GetDepth();

int GetNumberOfAddresses();

void Print();

void SetAddress(int index, int address);

int GetAddress(int index);

private:

bool DepthDecreasePossible();
int* CreateIndex(int numOfAddresses);    

// data members
int  _indexDepth;
int* _indexPointer;

};
