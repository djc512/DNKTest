package com.example.admin.dnktest;

/**
 * Created by admin on 2018/2/27.
 */

public class TestBean {

    private String name = "";
    private int age;
    private Family family;

    public Family getFamily() {
        return family;
    }

    public void setFamily(Family family) {
        this.family = family;
    }

    public String getName() {
        return name;
    }

    public void setName(String name) {
        this.name = name;
    }

    public int getAge() {
        return age;
    }

    public void setAge(int age) {
        this.age = age;
    }
}
