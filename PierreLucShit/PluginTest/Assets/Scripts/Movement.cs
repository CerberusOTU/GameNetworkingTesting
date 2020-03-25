using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Movement : MonoBehaviour {

    // Update is called once per frame
    void Update () {
        if (Input.GetKey (KeyCode.D))
            transform.localPosition += new Vector3 (0.1f, 0.0f, 0.0f);
        if (Input.GetKey (KeyCode.A))
            transform.localPosition += new Vector3 (-0.1f, 0.0f, 0.0f);
        if (Input.GetKey (KeyCode.W))
            transform.localPosition += new Vector3 (0.0f, 0.0f, 0.1f);
        if (Input.GetKey (KeyCode.S))
            transform.localPosition += new Vector3 (0.0f, 0.0f, -0.1f);

        if (Input.GetKey (KeyCode.LeftArrow))
            transform.Rotate (0f, -1f, 0f, Space.Self);
        if (Input.GetKey (KeyCode.RightArrow))
            transform.Rotate (0f, 1f, 0f, Space.Self);
    }
}