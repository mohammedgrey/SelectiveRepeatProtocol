//
// Generated file, do not edit! Created by nedtool 5.6 from MyMessage.msg.
//

#ifndef __MYMESSAGE_M_H
#define __MYMESSAGE_M_H

#if defined(__clang__)
#  pragma clang diagnostic ignored "-Wreserved-id-macro"
#endif
#include <omnetpp.h>

// nedtool version check
#define MSGC_VERSION 0x0506
#if (MSGC_VERSION!=OMNETPP_VERSION)
#    error Version mismatch! Probably this file was generated by an earlier version of nedtool: 'make clean' should help.
#endif



// cplusplus {{
#include <bitset>
typedef  std::bitset<8> bits;
// }}

/**
 * Class generated from <tt>MyMessage.msg:24</tt> by nedtool.
 * <pre>
 * packet MyMessage
 * {
 *     \@customize(true);  // see the generated C++ header for more info
 * 
 *       //header
 *     int id;
 *     double start_Time;
 * 
 *     //payload
 *     string M_Payload;
 *     int M_Type;
 * 
 *     //trailer
 *     int CRC;
 * 
 *     //piggybacking
 *     int p_id;
 *     int p_ack;
 * 
 * }
 * </pre>
 *
 * MyMessage_Base is only useful if it gets subclassed, and MyMessage is derived from it.
 * The minimum code to be written for MyMessage is the following:
 *
 * <pre>
 * class MyMessage : public MyMessage_Base
 * {
 *   private:
 *     void copy(const MyMessage& other) { ... }

 *   public:
 *     MyMessage(const char *name=nullptr, short kind=0) : MyMessage_Base(name,kind) {}
 *     MyMessage(const MyMessage& other) : MyMessage_Base(other) {copy(other);}
 *     MyMessage& operator=(const MyMessage& other) {if (this==&other) return *this; MyMessage_Base::operator=(other); copy(other); return *this;}
 *     virtual MyMessage *dup() const override {return new MyMessage(*this);}
 *     // ADD CODE HERE to redefine and implement pure virtual functions from MyMessage_Base
 * };
 * </pre>
 *
 * The following should go into a .cc (.cpp) file:
 *
 * <pre>
 * Register_Class(MyMessage)
 * </pre>
 */
class MyMessage_Base : public ::omnetpp::cPacket
{
  protected:
    int id;
    double start_Time;
    ::omnetpp::opp_string M_Payload;
    int M_Type;
    int CRC;
    int p_id;
    int p_ack;

  private:
    void copy(const MyMessage_Base& other);

  protected:
    // protected and unimplemented operator==(), to prevent accidental usage
    bool operator==(const MyMessage_Base&);
    // make constructors protected to avoid instantiation
//    MyMessage_Base(const char *name=nullptr, short kind=0);
//    MyMessage_Base(const MyMessage_Base& other);
    // make assignment operator protected to force the user override it
    MyMessage_Base& operator=(const MyMessage_Base& other);

  public:
    MyMessage_Base(const char *name=nullptr, short kind=0);
    MyMessage_Base(const MyMessage_Base& other);
    virtual ~MyMessage_Base();
    virtual MyMessage_Base *dup() const override {throw omnetpp::cRuntimeError("You forgot to manually add a dup() function to class MyMessage");}
    virtual void parsimPack(omnetpp::cCommBuffer *b) const override;
    virtual void parsimUnpack(omnetpp::cCommBuffer *b) override;

    // field getter/setter methods
    virtual int getId() const;
    virtual void setId(int id);
    virtual double getStart_Time() const;
    virtual void setStart_Time(double start_Time);
    virtual const char * getM_Payload() const;
    virtual void setM_Payload(const char * M_Payload);
    virtual int getM_Type() const;
    virtual void setM_Type(int M_Type);
    virtual int getCRC() const;
    virtual void setCRC(int CRC);
    virtual int getP_id() const;
    virtual void setP_id(int p_id);
    virtual int getP_ack() const;
    virtual void setP_ack(int p_ack);
};


#endif // ifndef __MYMESSAGE_M_H

