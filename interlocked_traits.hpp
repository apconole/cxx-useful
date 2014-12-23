namespace cxx_utils
{
    struct null_interlocked_trait
    {
        struct locked_exchange_type { };
        struct locked_exchange
        {
            locked_exchange(locked_exchange_type &e){}
            ~locked_exchange(){}
        };
    }
}
