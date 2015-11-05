#include <nan.h>

#include "orientc_reader.h"
#include "listener.h"
#include <iostream>
using namespace Orient;


void TrackerListener::startDocument(const char * name,size_t name_length) {
	v8::Local<v8::Object> cur = Nan::New<v8::Object>();
	if(!stack.empty()) {
		setValue(cur);
	} else
		obj = cur;
	this->stack.push_front(cur);

	if(name_length > 0)
		this->stack.front()->Set(Nan::New("@class").ToLocalChecked(), Nan::New(name,name_length).ToLocalChecked());
	this->stack.front()->Set(Nan::New("@type").ToLocalChecked(), Nan::New("d").ToLocalChecked());
}

void TrackerListener::endDocument() {
	this->stack.pop_front();
}

void TrackerListener::startField(const char * name,size_t name_length, OType type) {
	this->field_name = Nan::New(name,name_length).ToLocalChecked();
	this->type = type;
}

void TrackerListener::endField(const char * name,size_t name_length) {
}

void TrackerListener::stringValue(const char * value,size_t value_length) {
	setValue(Nan::New(value,value_length).ToLocalChecked());
}

void TrackerListener::intValue(long value) {
	setValue(v8::Number::New(isolate,value));
}

void TrackerListener::longValue(long long value) {
	setValue(v8::Number::New(isolate,value));
}

void TrackerListener::shortValue(short value) {
	setValue(v8::Number::New(isolate,value));
}

void TrackerListener::byteValue(char value) {
	setValue(v8::Number::New(isolate,value));
}

void TrackerListener::booleanValue(bool value) {
	setValue(v8::Boolean::New(isolate,value));
}

void TrackerListener::floatValue(float value) {
	setValue(v8::Number::New(isolate,value));
}

void TrackerListener::doubleValue(double value) {
	setValue(v8::Number::New(isolate,value));
}

void TrackerListener::binaryValue(const char * value, int length) {
	setValue(node::Buffer::New(isolate,(char *)value,length).ToLocalChecked());
}

void TrackerListener::dateValue(long long value) {
	setValue(v8::Date::New(isolate,value));
}

void TrackerListener::dateTimeValue(long long value) {
	setValue(v8::Date::New(isolate,value));
}

void TrackerListener::linkValue(struct Link &value) {
	v8::Local<v8::Object> cur = v8::Object::New(isolate);
	cur->Set(Nan::New("cluster").ToLocalChecked(), v8::Number::New(isolate,value.cluster));
	cur->Set(Nan::New("position").ToLocalChecked(), v8::Number::New(isolate,value.position));
	v8::Handle<v8::Value> handles[1];
	handles[0] = cur;
	setValue(ridFactory->NewInstance(1,handles));
}

void TrackerListener::startCollection(int size,OType type) {
	v8::Local<v8::Object> cur = v8::Array::New(isolate);
	if(type == LINKBAG && useRidBag) {
		v8::Handle<v8::Value> handles[1];
		handles[0] = v8::Null(isolate);
		v8::Local<v8::Object> bag = bagFactory->NewInstance(1,handles);
		bag->Set(Nan::New("_content").ToLocalChecked(), cur);
		bag->Set(Nan::New("_type").ToLocalChecked(), v8::Number::New(isolate,0));
		bag->Set(Nan::New("_size").ToLocalChecked(), v8::Number::New(isolate,size));
		setValue(bag);
	} else
		setValue(cur);
	this->stack.push_front(cur);
}

void TrackerListener::startMap(int size,OType type) {
	v8::Local<v8::Object> cur = v8::Object::New(isolate);
	setValue(cur);
	this->stack.push_front(cur);
}

void TrackerListener::mapKey(const char *key,size_t key_size) {
	this->field_name = Nan::New(key,key_size).ToLocalChecked();
}

void TrackerListener::ridBagTreeKey(long long fileId,long long pageIndex,long pageOffset) {
	v8::Handle<v8::Value> handles[1];
	handles[0] = v8::Null(isolate);
	v8::Local<v8::Object> bag = bagFactory->NewInstance(1,handles);
	bag->Set(Nan::New("_type").ToLocalChecked(), v8::Number::New(isolate,1));
	bag->Set(Nan::New("_fileId").ToLocalChecked(), v8::Number::New(isolate,fileId));
	bag->Set(Nan::New("_pageIndex").ToLocalChecked(), v8::Number::New(isolate,pageIndex));
	bag->Set(Nan::New("_pageOffset").ToLocalChecked(), v8::Number::New(isolate,pageOffset));
	bag->Set(Nan::New("_size").ToLocalChecked(), v8::Number::New(isolate,0));
	//TODO: check if the value is set in the correct place
	setValue(bag);
}

void TrackerListener::nullValue() {
	setValue(v8::Null(isolate));
}

void TrackerListener::endMap(OType type) {
	this->stack.pop_front();
}

void TrackerListener::endCollection(OType type) {
	this->stack.pop_front();
}

void TrackerListener::setValue(v8::Handle<v8::Value> value) {
	if(this->stack.front()->IsArray()){
		v8::Local<v8::Array> arr = v8::Local<v8::Array>::Cast(this->stack.front());
		arr->Set(arr->Length(),value);
	} else this->stack.front()->Set(this->field_name, value);
}


TrackerListener::TrackerListener(v8::Local<v8::Function> ridFactory ,v8::Local<v8::Function > bagFactory ,v8::Isolate * isolate,bool useRidBag):ridFactory(ridFactory),bagFactory(bagFactory),isolate(isolate),useRidBag(useRidBag) {
}

TrackerListener::~TrackerListener() {
}

