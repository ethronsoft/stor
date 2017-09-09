from ethronsoft.pystor.store import *
import tempfile
import json
import os

def test_db_creation():
    tmp = tempfile.gettempdir()
    db = Store(Cstor.load(), db_home=tmp, db_name="test_db", temp=True)
    assert db
    assert db.home == tmp + "/test_db/"
    assert os.path.exists(db.home)

def test_db_collection_creation():
    tmp = tempfile.gettempdir()
    db = Store(Cstor.load(), db_home=tmp, db_name="test_db", temp=True)
    coll = db["test_collection"]
    assert coll.name == "test_collection"
    assert "test_collection" in db
    assert os.path.exists(db.home + coll.name)

def test_db_collection_removal():
    tmp = tempfile.gettempdir()
    db = Store(Cstor.load(), db_home=tmp, db_name="test_db", temp=True)
    coll = db["test_collection"]
    db.remove(coll.name)
    assert coll.name not in db
    assert not os.path.exists(db.home + coll.name)

def test_db_collection_doc_insertion():
    tmp = tempfile.gettempdir()
    db = Store(Cstor.load(), db_home=tmp, db_name="test_db", temp=True)
    coll = db["test_collection"]
    doc = Document(Cstor.load(), "[1,2,3]")
    coll.put(doc)
    assert doc.id in coll

def test_db_collection_doc_removal():
    tmp = tempfile.gettempdir()
    db = Store(Cstor.load(), db_home=tmp, db_name="test_db", temp=True)
    coll = db["test_collection"]
    doc = Document(Cstor.load(), "[1,2,3]")
    coll.put(doc)
    coll.remove(doc.id)
    assert not doc.id in coll

def test_db_collection_doc_get_by_id():
    tmp = tempfile.gettempdir()
    db = Store(Cstor.load(), db_home=tmp, db_name="test_db", temp=True)
    coll = db["test_collection"]
    doc = Document(Cstor.load(), "[1,2,3]")
    coll.put(doc)
    doc2 = coll[doc.id]
    assert doc.id == doc.id
    assert doc.value == doc.value

# def test_db_collection_add_index():
#    TODO

# def test_db_collection_doc_query():
#    TODO
#     tmp = tempfile.gettempdir()
#     db = Store(Cstor.load(), db_home=tmp, db_name="test_db", temp=True)
#     coll = db["test_collection"]
#     coll.add_index("a")
#     obj = {"a": 1, "b": 2}
#     doc = Document(Cstor.load(), json.dumps(obj))
#     coll.put(doc)
#     res = coll.find(json.dumps({"$eq": {"a": 1}}))
#     assert len(res) == 1
#     assert res[0].value == obj


