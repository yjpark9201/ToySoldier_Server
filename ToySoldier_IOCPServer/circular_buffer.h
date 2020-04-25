#pragma once
;
#include <iostream>
template <class T>


class Circular_buffer {
public:
	explicit Circular_buffer(size_t size) :
		begin((new T[size])),
		End(size) { }

	explicit Circular_buffer(char* buffer, size_t size) :
		begin(buffer),
		End(size) {
	}


	~Circular_buffer();


	void put(T item);
	 
	T* GetBegin() { return begin; }
	T get();
	void reset();
	bool empty() const;
	bool full() const;
	size_t capacity() const;
	size_t remain() const;
	T* GetRecordablePoint() const;   //  Socket이 Recv 가능한 지점
	size_t GetRecordableSize() const;    // Socket이 한번에 Recv가능한 범위까지의 길이
	void   update_head(size_t recored);
	void   update_tail(size_t recored);
	size_t FromTailToEnd() { return End - tail_; }
	void Copy(T* buf, int len);
	T* Ref(T* buf, int len);
	//size_t GetRecordable(int& len) // len만큼의 길이를 
private:
	//std::mutex mutex_;
	T*  begin;
	size_t head_ = 0;
	size_t tail_ = 0;
	const size_t End; //end
	bool full_ = 0;

	//	unsigned char *m_load_ptr, *m_consume_ptr; 
	//	unsigned char *m_buff_end;
 //unsigned char *m_buff; 
	//	int m_max_load, m_max_consume, m_data_in_buffer;

};

template <class T>
Circular_buffer<T>::~Circular_buffer() {
	std::cout << "링버퍼 소멸 - 메모리 주소" << begin <<std::endl;
	delete begin;
}

template <class T>
void Circular_buffer<T>::reset()
{
	//	std::lock_guard<std::mutex> lock(mutex_);
	head_ = tail_;
	full_ = false;
}


template <class T>
bool Circular_buffer<T>::empty() const
{
	//if head and tail are equal, we are empty
	return (!full_ && (head_ == tail_));
}

template <class T>
bool Circular_buffer<T>::full() const
{
	//If tail is ahead the head by 1, we are full
	return full_;
}


template <class T>
size_t Circular_buffer<T> ::capacity() const
{
	return End;
}

template <class T>
size_t Circular_buffer<T> ::remain() const
{
	size_t size = End;

	if (!full_)
	{
		if (head_ >= tail_)
		{
			size = head_ - tail_;
		}
		else
		{
			size = End + head_ - tail_;
		}
	}

	return size;
}

template <class T>
T*  Circular_buffer<T> ::GetRecordablePoint() const {

	return begin + head_;

}

template <class T>
size_t  Circular_buffer<T> ::GetRecordableSize() const {

	if (head_ >= tail_) return End - head_;
	else return tail_ - head_;

}

template <class T>
void  Circular_buffer<T> ::Copy(T* buf, int len) {
	if ((head_ >= tail_) || (End - tail_ >= len)) {
		MoveMemory(buf, begin + tail_, len);
		tail_ = tail_ + len;
	}

	else {
		MoveMemory(buf, begin + tail_, End - tail_);
		MoveMemory(buf + End - tail_, begin, len - End + tail_);
		tail_ = len - End + tail_;// ( tail_ + len )% End; 와 같음.
	}


}


template <class T>
void  Circular_buffer<T> ::update_tail(size_t len) {
	if ((head_ >= tail_) || (End - tail_ >= len)) {;
		tail_ = tail_ + len;
	}

	else {
		tail_ = len - End + tail_;// ( tail_ + len )% End; 와 같음.
	}


}


template <class T>
T*  Circular_buffer<T> ::Ref(T* buf, int len) {
	if ((head_ >= tail_) || (End - tail_ >= len)) {
		MoveMemory(buf, begin + tail_, len);
	}

	else {
		MoveMemory(buf, begin + tail_, End - tail_);
		MoveMemory(buf + End - tail_, begin, len - End + tail_);
	}

	return buf;


}

template <class T>
void Circular_buffer<T> ::put(T item)
{
	//std::lock_guard<std::mutex> lock(mutex_);

	begin[head_] = item;

	if (full_)
	{
		tail_ = (tail_ + 1) % End;
	}

	head_ = (head_ + 1) % End;

	full_ = head_ == tail_;
}


template <class T>
void Circular_buffer<T> ::update_head(size_t recored) {

	//std::lock_guard<std::mutex> lock(mutex_);


	//if (full_)
	//{
	//	tail_ = (tail_ + recored) % End;
	//}

	head_ = (head_ + recored) % End;

	//full_ = head_ == tail_;

}


