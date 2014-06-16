using UnityEngine;
using System.Collections;

public class HelloWorld : MonoBehaviour {
	
	public float moveSpeed = 2;
	
	// Use this for initialization
	void Start () {
	
	}
	
	// Update is called once per frame
	void Update () {
	    float moveX = Input.GetAxis("Horizontal") * moveSpeed * Time.deltaTime;
        float moveZ = Input.GetAxis("Vertical") * moveSpeed * Time.deltaTime;
        transform.Translate( moveX, 0, moveZ );
	}
}
