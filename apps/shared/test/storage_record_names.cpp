#include <quiz.h>
#include "../record_name_helper.h"

using namespace Ion;

namespace Shared {

void createTestRecordWithErrorStatus(const char * baseName, const char * extension, const char * data = nullptr, Storage::Record::ErrorStatus testError = Storage::Record::ErrorStatus::None) {
  if (data == nullptr) {
    data = "test";
  }
  size_t dataSize = strlen(data) + 1;
  Storage::Record::ErrorStatus error = Storage::sharedStorage()->createRecordWithExtension(baseName, extension, data, dataSize);
  quiz_assert(error == testError);
}

Storage::Record getRecord(const char * baseName, const char * extension) {
  return Storage::sharedStorage()->recordBaseNamedWithExtension(baseName, extension);
}

bool isDataOfRecord(const char * baseName, const char * extension, const char * data) {
  const char * recordData = reinterpret_cast<const char *>(getRecord(baseName, extension).value().buffer);
  return strcmp(recordData, data) == 0;
}

QUIZ_CASE(record_name_helper) {
  Shared::RecordNameHelper recordNameHelper;
  Ion::Storage::sharedStorage()->setRecordNameHelper(&recordNameHelper);

  const char * varName0 = "A";
  const char * varName1 = "A1";
  const char * varName2 = "A10";
  const char * data0 = "abcdefgh";
  const char * data1 = "Bonjour Hello";

  // Test if record overrides itself if same name and extension
  createTestRecordWithErrorStatus(varName1, Storage::expExtension);
  createTestRecordWithErrorStatus(varName1, Storage::expExtension, data1);
  quiz_assert(isDataOfRecord(varName1, Storage::expExtension, data1));

  // Test if record does not overrides if name slightly differs
  createTestRecordWithErrorStatus(varName0, Storage::expExtension);
  quiz_assert(isDataOfRecord(varName1, Storage::expExtension, data1));

  createTestRecordWithErrorStatus(varName2, Storage::expExtension);
  quiz_assert(isDataOfRecord(varName1, Storage::expExtension, data1));

  Storage::sharedStorage()->destroyAllRecords();

  // Test if record overrides itself if same name and more important extension
  createTestRecordWithErrorStatus(varName0, Storage::expExtension);
  createTestRecordWithErrorStatus(varName1, Storage::matExtension);
  createTestRecordWithErrorStatus(varName2, Storage::lisExtension);

  createTestRecordWithErrorStatus(varName0, Storage::funcExtension);
  createTestRecordWithErrorStatus(varName1, Storage::seqExtension);
  createTestRecordWithErrorStatus(varName2, Storage::funcExtension);

  quiz_assert(getRecord(varName0, Storage::expExtension).isNull());
  quiz_assert(!getRecord(varName0, Storage::funcExtension).isNull());
  
  quiz_assert(getRecord(varName1, Storage::matExtension).isNull());
  quiz_assert(!getRecord(varName1, Storage::seqExtension).isNull());
  
  quiz_assert(getRecord(varName2, Storage::lisExtension).isNull());
  quiz_assert(!getRecord(varName2, Storage::funcExtension).isNull());

  // Test if record with more important extension can't be overriden
  createTestRecordWithErrorStatus(varName0, Storage::expExtension, data0, Storage::Record::ErrorStatus::NameTaken);
  createTestRecordWithErrorStatus(varName1, Storage::matExtension, data0, Storage::Record::ErrorStatus::NameTaken);
  createTestRecordWithErrorStatus(varName2, Storage::lisExtension, data0, Storage::Record::ErrorStatus::NameTaken);

  Storage::sharedStorage()->destroyAllRecords();

  // Test if record with same precedence extension can be overriden
  createTestRecordWithErrorStatus(varName0, Storage::expExtension);
  createTestRecordWithErrorStatus(varName0, Storage::lisExtension);
  quiz_assert(getRecord(varName0, Storage::expExtension).isNull());

  // Test if record with non-competing extension can always be created
  const char * testExtension = "py";
  createTestRecordWithErrorStatus(varName0, testExtension);
  quiz_assert(!getRecord(varName0, Storage::lisExtension).isNull());
  quiz_assert(!getRecord(varName0, testExtension).isNull());

  Storage::sharedStorage()->destroyAllRecords();

  createTestRecordWithErrorStatus(varName0, testExtension);
  createTestRecordWithErrorStatus(varName0, Storage::expExtension);
  quiz_assert(!getRecord(varName0, Storage::expExtension).isNull());
  quiz_assert(!getRecord(varName0, testExtension).isNull());

  Storage::sharedStorage()->destroyAllRecords();

  // Test if reserved names are correctly handled

    // Regression/stats reserved names
  createTestRecordWithErrorStatus("X1", Storage::funcExtension);
  createTestRecordWithErrorStatus("Y1", Storage::funcExtension);
  createTestRecordWithErrorStatus("V2", Storage::funcExtension);
  createTestRecordWithErrorStatus("N3", Storage::funcExtension);
  createTestRecordWithErrorStatus("Y5", Storage::funcExtension);
  createTestRecordWithErrorStatus("N", Storage::funcExtension);

  createTestRecordWithErrorStatus("X1", Storage::lisExtension);
  quiz_assert(getRecord("X1", Storage::funcExtension).isNull());
  createTestRecordWithErrorStatus("Y1", Storage::lisExtension);
  quiz_assert(getRecord("Y1", Storage::funcExtension).isNull()); 
  createTestRecordWithErrorStatus("V2", Storage::lisExtension);
  quiz_assert(getRecord("V2", Storage::funcExtension).isNull());
  createTestRecordWithErrorStatus("N3", Storage::lisExtension);
  quiz_assert(getRecord("N3", Storage::funcExtension).isNull());
  createTestRecordWithErrorStatus("Y5", Storage::lisExtension, data0, Storage::Record::ErrorStatus::NameTaken);
  quiz_assert(!getRecord("Y5", Storage::funcExtension).isNull());
  createTestRecordWithErrorStatus("N", Storage::lisExtension, data0, Storage::Record::ErrorStatus::NameTaken);
  quiz_assert(!getRecord("N", Storage::funcExtension).isNull());

  createTestRecordWithErrorStatus("X1", Storage::funcExtension, data0, Storage::Record::ErrorStatus::NameTaken);
  createTestRecordWithErrorStatus("Y1", Storage::funcExtension, data0, Storage::Record::ErrorStatus::NameTaken);
  createTestRecordWithErrorStatus("V2", Storage::funcExtension, data0, Storage::Record::ErrorStatus::NameTaken);
  createTestRecordWithErrorStatus("N3", Storage::funcExtension, data0, Storage::Record::ErrorStatus::NameTaken);

    // Sequences reserved names
  createTestRecordWithErrorStatus("u", Storage::funcExtension);
  createTestRecordWithErrorStatus("v", Storage::funcExtension);
  createTestRecordWithErrorStatus("w", Storage::funcExtension);

  createTestRecordWithErrorStatus("u", Storage::seqExtension);
  quiz_assert(getRecord("u", Storage::funcExtension).isNull());
  createTestRecordWithErrorStatus("v", Storage::seqExtension);
  quiz_assert(getRecord("v", Storage::funcExtension).isNull());
  createTestRecordWithErrorStatus("w", Storage::seqExtension);
  quiz_assert(getRecord("w", Storage::funcExtension).isNull());

  createTestRecordWithErrorStatus("u", Storage::funcExtension, data0, Storage::Record::ErrorStatus::NameTaken);
  createTestRecordWithErrorStatus("v", Storage::funcExtension, data0, Storage::Record::ErrorStatus::NameTaken);
  createTestRecordWithErrorStatus("w", Storage::funcExtension, data0, Storage::Record::ErrorStatus::NameTaken);
  createTestRecordWithErrorStatus("u", testExtension);

  Storage::sharedStorage()->destroyAllRecords();
  Ion::Storage::sharedStorage()->setRecordNameHelper(nullptr);
}

}