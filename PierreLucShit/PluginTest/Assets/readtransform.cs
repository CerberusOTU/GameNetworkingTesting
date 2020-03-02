using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class readtransform : MonoBehaviour
{

    Vector3 position;
    int clientID;
    Wrapper wrapper;
    // Start is called before the first frame update
    void Start()
    {

    }

    // Update is called once per frame
    void Update()
    {
        wrapper.ReadTransform(ref position, ref clientID);
        this.transform.position = position;
    }
}
