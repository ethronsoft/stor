import sys, os
import copy

from ethronsoft.pystor.document import *

def test_copy():
    doc = Document(Cstor.load(), "{}")
    deep = copy.deepcopy(doc)
    assert doc.id != deep.id

    doc.from_json(json.dumps([1, 2, 3]))
    assert doc.to_json() != deep.to_json()

def test_object():
    doc = Document(Cstor.load(), "{}")
    assert doc.type == NodeType.OBJECT
    assert doc is not None
    assert len(doc.id) > 0
    assert doc.to_json() == "{}"
    assert doc.value == {}

    obj = {"a": 1, "b": {"z": [1,2,3]}}

    doc.value = obj
    assert doc.value == obj
    print doc.value
    assert json.loads(doc.to_json()) == obj

    assert len(doc) == 2
    assert len(doc["b"]) == 1
    assert len(doc["b"]["z"]) == 3
    assert 1 not in doc
    assert "a" in doc
    assert "b" in doc
    assert "motherfucker" in doc

    assert "z" in doc["b"]
    assert 1 in doc["b"]["z"]
    assert 4 not in doc["b"]["z"]

    assert doc["a"].value == 1
    assert doc["b"].value == obj["b"]
    assert doc["b"]["z"].value == obj["b"]["z"]

def test_array():
    doc = Document(Cstor.load(), "[]")
    assert doc is not None
    assert doc.type == NodeType.ARRAY
    assert len(doc.id) > 0
    assert doc.to_json() == "[]"
    assert doc.value == []

    doc.value = [1, 2, "a"]
    assert doc.value == [1, 2, "a"]
    assert json.loads(doc.to_json()) == [1, 2, "a"]

    assert len(doc) == 3
    assert 1 in doc
    assert 4 not in doc
    assert "a" in doc
    assert "b" not in doc

    assert doc[0].value == 1
    assert doc[1].value == 2
    assert doc[2].value == "a"



def test_value():
    doc = Document(Cstor.load(), "1")
    assert doc is not None
    assert len(doc.id) > 0
    assert doc.to_json() == "1"
    assert doc.value == 1



